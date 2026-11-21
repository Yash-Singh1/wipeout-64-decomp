#!/usr/bin/env python3
"""Compile a Wipeout 64 C source file to a big-endian MIPS ELF object.

Cross-platform pipeline (Linux x86_64/arm64, macOS arm64/x64, Windows):
  cppn64.exe -> cc1n64.exe (-O2 -G0 -mips3 -mcpu=4300 -mgas -meb) -> asn64.exe (-q -G0)
    (native on Windows; via wibo-macos/wibo/wine on macOS and Linux)
  -> psyq-obj-parser -b + mips-linux-gnu-objcopy --rename-section .rdata=.rodata
    (native on host when mips-linux-gnu binutils + host-compatible psyq-obj-parser
     are available, otherwise inside a long-lived Docker container)

Environment overrides:
  WIPEOUT64_TOOLCHAIN       Path to toolchains/ directory (default: <repo>/toolchains)
  WIPEOUT64_WIBO            Path to Win32 runner (wibo-macos / wibo / wine; empty on Windows)
  WIPEOUT64_POP             Path to native psyq-obj-parser executable
  WIPEOUT64_BINUTILS_PREFIX Binutils prefix on host (default: mips-linux-gnu-)
  WIPEOUT64_USE_DOCKER      auto (default) | 1 | 0
  WIPEOUT64_IMAGE           Docker image name (default: wipeout64-tools:latest or wipeoutxl:latest)
"""
import argparse
import hashlib
import io
import os
import platform
import re
import shlex
import shutil
import struct
import subprocess
import sys
import tarfile
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TOOLCHAIN = Path(os.environ.get("WIPEOUT64_TOOLCHAIN", ROOT / "toolchains")).resolve()
SN64 = TOOLCHAIN / "sn272_0006"
CPP = SN64 / "cppn64.exe"
CC1 = SN64 / "cc1n64.exe"
ASN = SN64 / "asn64.exe"
BINUTILS_PREFIX = os.environ.get("WIPEOUT64_BINUTILS_PREFIX", "mips-linux-gnu-")

_ROOT_HASH = hashlib.sha256(str(ROOT.resolve()).encode("utf-8")).hexdigest()[:8]
CONTAINER_NAME = f"wipeout64-cc-{_ROOT_HASH}"

_BINUTILS_MODE_CACHE = None
_DOCKER_IMAGE_CACHE = None
_WIBO_CACHE = None
_WIBO_RESOLVED = False

INCLUDE_ASM_RE = re.compile(
    r'^\s*INCLUDE_ASM\s*\(\s*"([^"]+)"\s*,\s*([A-Za-z0-9_.$]+)\s*\)\s*;?\s*$'
)
INCLUDE_ASM_CPP_RE = re.compile(
    r'__WO64_INCLUDE_ASM_CALL__\s*\(\s*"([^"]+)"\s*,\s*([A-Za-z0-9_.$]+)\s*\)\s*;?'
)


def _is_host_executable_format(path):
    """Return True if binary at `path` matches the host OS/architecture format."""
    path = Path(path)
    if not path.is_file():
        return False
    try:
        hdr = path.read_bytes()[:20]
    except OSError:
        return False
    if len(hdr) < 4:
        return False

    if sys.platform == "win32":
        return hdr[:2] == b"MZ"
    if sys.platform == "darwin":
        # Mach-O 32/64 or Universal FAT binary
        magic = struct.unpack(">I", hdr[:4])[0]
        return magic in (0xFEEDFACE, 0xCEFAEDFE, 0xFEEDFACF, 0xCFFAEDFE, 0xCAFEBABE, 0xBEBAFECA)
    if sys.platform.startswith("linux"):
        if hdr[:4] != b"\x7fELF" or len(hdr) < 20:
            return False
        ei_data = hdr[5]
        e_machine = struct.unpack("<H" if ei_data == 1 else ">H", hdr[18:20])[0]
        mach = platform.machine().lower()
        if mach in ("x86_64", "amd64"):
            return e_machine in (3, 62)  # EM_386 or EM_X86_64
        if mach in ("i386", "i686"):
            return e_machine == 3
        if mach in ("aarch64", "arm64"):
            return e_machine == 183  # EM_AARCH64
        return True
    return False


def resolve_wibo():
    """Return Path to Win32 runner (wibo/wine) on POSIX, or None on native Windows."""
    global _WIBO_CACHE, _WIBO_RESOLVED
    if _WIBO_RESOLVED:
        return _WIBO_CACHE
    _WIBO_RESOLVED = True

    env_wibo = os.environ.get("WIPEOUT64_WIBO")
    if env_wibo is not None:
        _WIBO_CACHE = Path(env_wibo) if env_wibo.strip() else None
        return _WIBO_CACHE

    if sys.platform == "win32":
        _WIBO_CACHE = None
        return None

    cands = []
    if sys.platform == "darwin":
        cands.extend([TOOLCHAIN / "bin" / "wibo-macos", TOOLCHAIN / "bin" / "wibo"])
    else:
        cands.extend([TOOLCHAIN / "bin" / "wibo"])

    for cand in cands:
        if _is_host_executable_format(cand):
            _WIBO_CACHE = cand
            return _WIBO_CACHE

    for cmd in ("wibo", "wine"):
        found = shutil.which(cmd)
        if found:
            _WIBO_CACHE = Path(found)
            return _WIBO_CACHE

    raise FileNotFoundError(
        "No Win32 runner found for SN64 executables (cppn64.exe/cc1n64.exe/asn64.exe). "
        "Install wibo or wine on PATH, place a host-compatible wibo binary in toolchains/bin/, "
        "or set WIPEOUT64_WIBO."
    )


def win32_cmd(exe, *args):
    """Build argv to execute a Win32 SN64 binary on any OS."""
    exe_p = Path(exe)
    if not exe_p.is_file():
        raise FileNotFoundError(
            f"Missing SN64 toolchain binary: {exe_p}\n"
            f"Place SN64 2.72 binaries in {SN64} or set WIPEOUT64_TOOLCHAIN."
        )
    runner = resolve_wibo()
    if runner is None:
        return [str(exe_p), *args]
    return [str(runner), str(exe_p), *args]


def resolve_native_pop():
    """Return Path to a host-runnable psyq-obj-parser binary, or None."""
    env_pop = os.environ.get("WIPEOUT64_POP")
    if env_pop:
        p = Path(env_pop)
        if p.is_file():
            return p
    cands = []
    if sys.platform == "win32":
        cands.append(TOOLCHAIN / "bin" / "psyq-obj-parser.exe")
    elif sys.platform == "darwin":
        cands.extend([
            TOOLCHAIN / "bin" / "psyq-obj-parser-macos",
            TOOLCHAIN / "bin" / "psyq-obj-parser",
        ])
    else:
        cands.append(TOOLCHAIN / "bin" / "psyq-obj-parser")
    for cand in cands:
        if _is_host_executable_format(cand):
            return cand
    found = shutil.which("psyq-obj-parser")
    if found and _is_host_executable_format(found):
        return Path(found)
    return None


def resolve_native_binutils():
    """Return dict(as, ld, objcopy) if host MIPS binutils are installed, else None."""
    tools = {}
    for k in ("as", "ld", "objcopy"):
        found = shutil.which(f"{BINUTILS_PREFIX}{k}")
        if not found:
            return None
        tools[k] = found
    return tools


def use_docker_binutils():
    """Decide whether to run binutils/psyq-obj-parser via Docker or natively on host."""
    global _BINUTILS_MODE_CACHE
    if _BINUTILS_MODE_CACHE is not None:
        return _BINUTILS_MODE_CACHE

    pref = os.environ.get("WIPEOUT64_USE_DOCKER", "auto").strip().lower()
    if pref in ("1", "true", "yes"):
        _BINUTILS_MODE_CACHE = True
        return True
    if pref in ("0", "false", "no"):
        if resolve_native_binutils() is None:
            raise RuntimeError(
                f"WIPEOUT64_USE_DOCKER=0 set, but {BINUTILS_PREFIX}{{as,ld,objcopy}} were not found on PATH."
            )
        if resolve_native_pop() is None:
            raise RuntimeError(
                "WIPEOUT64_USE_DOCKER=0 set, but no host-compatible psyq-obj-parser was found "
                "(set WIPEOUT64_POP or place it in toolchains/bin/)."
            )
        _BINUTILS_MODE_CACHE = False
        return False

    # auto: prefer native host tools when both MIPS binutils and host-compatible psyq-obj-parser exist
    if resolve_native_binutils() is not None and resolve_native_pop() is not None:
        _BINUTILS_MODE_CACHE = False
        return False
    _BINUTILS_MODE_CACHE = True
    return True


def resolve_docker_image():
    global _DOCKER_IMAGE_CACHE
    if _DOCKER_IMAGE_CACHE is not None:
        return _DOCKER_IMAGE_CACHE
    env_img = os.environ.get("WIPEOUT64_IMAGE")
    if env_img:
        _DOCKER_IMAGE_CACHE = env_img
        return _DOCKER_IMAGE_CACHE
    for img in ("wipeout64-tools:latest", "wipeoutxl:latest"):
        r = subprocess.run(["docker", "image", "inspect", img], capture_output=True)
        if r.returncode == 0:
            _DOCKER_IMAGE_CACHE = img
            return _DOCKER_IMAGE_CACHE
    _DOCKER_IMAGE_CACHE = "wipeout64-tools:latest"
    return _DOCKER_IMAGE_CACHE


def _container_pop_path():
    """Return path to psyq-obj-parser inside the Docker container."""
    try:
        rel = (TOOLCHAIN / "bin" / "psyq-obj-parser").relative_to(ROOT).as_posix()
        return f"/work/{rel}"
    except ValueError:
        return "/toolchain/bin/psyq-obj-parser"


def _docker_mount_args():
    args = ["-v", f"{ROOT.resolve().as_posix()}:/work"]
    try:
        TOOLCHAIN.relative_to(ROOT)
    except ValueError:
        args.extend(["-v", f"{TOOLCHAIN.as_posix()}:/toolchain"])
    return args


def ensure_container():
    if shutil.which("docker") is None:
        raise RuntimeError(
            "Docker is not installed or not on PATH, and native MIPS binutils + psyq-obj-parser "
            "were not detected on host. Either install Docker and build `docker build -t wipeout64-tools .`, "
            "or install mips-linux-gnu binutils + native psyq-obj-parser."
        )
    res = subprocess.run(
        ["docker", "inspect", "-f", "{{.State.Running}}", CONTAINER_NAME],
        capture_output=True,
        text=True,
    )
    if res.returncode == 0 and res.stdout.strip() == "true":
        return
    subprocess.run(["docker", "rm", "-f", CONTAINER_NAME], capture_output=True)
    img = resolve_docker_image()
    r = subprocess.run(
        [
            "docker",
            "run",
            "-d",
            "--rm",
            "--name",
            CONTAINER_NAME,
            *_docker_mount_args(),
            "-w",
            "/work",
            img,
            "sleep",
            "infinity",
        ],
        capture_output=True,
        text=True,
    )
    if r.returncode != 0:
        raise RuntimeError(
            f"Failed to start container {CONTAINER_NAME} (image {img}): {r.stderr}\n"
            f"Build the image with: docker build -t wipeout64-tools ."
        )


def docker_exec(cmd_str, input_bytes=None):
    ensure_container()
    r = subprocess.run(
        ["docker", "exec", "-i", "-w", "/work", CONTAINER_NAME, "sh", "-eu", "-c", cmd_str],
        input=input_bytes,
        capture_output=True,
    )
    if r.returncode != 0:
        raise RuntimeError(
            f"Container command failed ({r.returncode}):\n{r.stderr.decode('utf-8', 'replace')}"
        )
    return r.stdout


def _clean_host_env():
    """Minimal environment for Win32/SN64 compilers (avoids Make env-block bugs in cppn64.exe)."""
    if sys.platform == "win32":
        keep = {
            "PATH", "SYSTEMROOT", "WINDIR", "COMSPEC", "TEMP", "TMP",
            "PATHEXT", "USERPROFILE", "APPDATA", "LOCALAPPDATA", "HOMEDRIVE", "HOMEPATH",
        }
        return {k: v for k, v in os.environ.items() if k.upper() in keep}
    keep = {"PATH", "HOME", "USER", "TMPDIR", "LANG", "LC_ALL", "WINEPREFIX", "WINEDEBUG"}
    return {k: v for k, v in os.environ.items() if k in keep}


def _short_tmp_base():
    """Use short /tmp paths on POSIX so 1997 Win32 cppn64.exe never overflows path buffers."""
    if sys.platform != "win32" and os.path.isdir("/tmp"):
        return "/tmp"
    return None


def run_host(args, cwd=None, binary=False):
    clean_env = _clean_host_env()
    for attempt in range(3):
        r = subprocess.run(
            args,
            cwd=cwd,
            env=clean_env,
            capture_output=True,
            text=not binary,
        )
        if r.returncode == 0:
            return r
        if binary:
            msg = (r.stdout or b"").decode("utf-8", "replace") + (r.stderr or b"").decode("utf-8", "replace")
        else:
            msg = (r.stdout or "") + (r.stderr or "")
        # Retry occasional silent crashes from Win32 runners (e.g. wibo-macos under Rosetta)
        if not msg.strip() and attempt < 2:
            continue
        raise RuntimeError(f"Command failed (rc={r.returncode}): {' '.join(map(str, args))}\n{msg}")
    return r


def lnk_to_elf_bytes(lnk_bytes):
    """Convert SN LNK object bytes -> big-endian MIPS ELF with .rdata renamed to .rodata."""
    if not use_docker_binutils():
        pop = resolve_native_pop()
        bu = resolve_native_binutils()
        with tempfile.TemporaryDirectory(prefix="w64pop_", dir=_short_tmp_base()) as tmp:
            tp = Path(tmp)
            in_obj = tp / "in.obj"
            out_o = tp / "out.o"
            out2_o = tp / "out2.o"
            in_obj.write_bytes(lnk_bytes)
            run_host([str(pop), str(in_obj), "-b", "-o", str(out_o)])
            run_host([bu["objcopy"], "--rename-section", ".rdata=.rodata", str(out_o), str(out2_o)])
            return out2_o.read_bytes()

    pop_in_c = shlex.quote(_container_pop_path())
    return docker_exec(
        f'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
        f'cat > "$d/in.obj" && '
        f'{pop_in_c} "$d/in.obj" -b -o "$d/out.o" >/dev/null && '
        f'{BINUTILS_PREFIX}objcopy --rename-section .rdata=.rodata "$d/out.o" "$d/out2.o" && '
        f'cat "$d/out2.o"',
        input_bytes=lnk_bytes,
    )


def compile_c_to_obj_bytes(src_text, src_dir, version, opt, extra_cc1, extra_cpp, save_prefix=None):
    """Compile C source text (without INCLUDE_ASM) -> big-endian ELF .o bytes."""
    ver_def = "-DVERSION_US" if version == "us" else "-DVERSION_EU"
    with tempfile.TemporaryDirectory(prefix="w64_", dir=_short_tmp_base()) as tmp:
        tmp_p = Path(tmp)
        c_path = tmp_p / "in.c"
        i_path = tmp_p / "in.i"
        s_path = tmp_p / "out.s"
        obj_path = tmp_p / "out.obj"
        c_path.write_text(src_text, encoding="utf-8")

        libreultra_inc = TOOLCHAIN / "libreultra" / "include" / "2.0I"
        extra_inc = [f"-I{libreultra_inc}"] if libreultra_inc.is_dir() else []
        cpp_cmd = win32_cmd(
            CPP,
            "-lang-c",
            "-undef",
            "-D__GNUC__=2",
            "-D_MIPS_SZLONG=32",
            "-D_MIPS_SZINT=32",
            "-D_LANGUAGE_C",
            "-DLANGUAGE_C",
            "-Dmips",
            "-D__mips__",
            "-DMIPSEB",
            "-D_MIPSEB",
            "-DF3DEX_GBI",
            ver_def,
            f"-I{ROOT / 'include'}",
            *extra_inc,
            f"-I{src_dir}",
            *extra_cpp,
            str(c_path),
            str(i_path),
        )
        run_host(cpp_cmd)

        cc1_cmd = win32_cmd(
            CC1,
            "-quiet",
            opt,
            "-G0",
            "-mips3",
            "-mcpu=4300",
            "-mgas",
            "-meb",
            *extra_cc1,
            str(i_path),
            "-o",
            str(s_path),
        )
        run_host(cc1_cmd)

        if save_prefix:
            shutil.copyfile(s_path, f"{save_prefix}.s")

        asn_cmd = win32_cmd(
            ASN,
            "-q",
            "-G0",
            "-o",
            str(obj_path),
            str(s_path),
        )
        run_host(asn_cmd)

        lnk_bytes = obj_path.read_bytes()

    return lnk_to_elf_bytes(lnk_bytes)


def elf_section_size_and_syms(elf_bytes, sym_prefix=None):
    """Return (rodata_size, {name: st_value}) from a big-endian 32-bit ELF object."""
    e_shoff = struct.unpack(">I", elf_bytes[32:36])[0]
    e_shentsize, e_shnum, e_shstrndx = struct.unpack(">HHH", elf_bytes[46:52])
    shdrs = []
    for i in range(e_shnum):
        off = e_shoff + i * e_shentsize
        shdrs.append(struct.unpack(">10I", elf_bytes[off:off + 40]))
    shstr_off = shdrs[e_shstrndx][4]

    def _cstr(base, idx):
        end = elf_bytes.index(b"\x00", base + idx)
        return elf_bytes[base + idx:end].decode("ascii", errors="replace")

    ro_size = 0
    symtab = None
    strtab_off = 0
    for sh in shdrs:
        nm = _cstr(shstr_off, sh[0])
        if nm in (".rodata", ".rdata"):
            ro_size = sh[5]
        elif sh[1] == 2:  # SHT_SYMTAB
            symtab = sh
            strtab_off = shdrs[sh[6]][4]
    syms = {}
    if sym_prefix and symtab:
        sym_off, sym_sz, sym_ent = symtab[4], symtab[5], symtab[9]
        for p in range(sym_off, sym_off + sym_sz, sym_ent):
            st_name, st_value = struct.unpack(">II", elf_bytes[p:p + 8])
            nm = _cstr(strtab_off, st_name)
            if nm.startswith(sym_prefix):
                syms[nm] = st_value
    return ro_size, syms


def _fix_rdata_align3_in_s(s_text, ro_len, assemble_fn):
    """When a C chunk's .rdata starts at ro_len % 8 == 4, replace .align 3 inside .rdata
    with exact 4-byte-phase-aware padding (.align 2 [+ .word 0])."""
    if ro_len % 8 == 0:
        return s_text

    lines = s_text.splitlines(keepends=True)
    in_rdata = False
    al3_indices = []
    for i, ln in enumerate(lines):
        m_sec = re.match(r'^\s*\.(text|rdata|data|sdata|bss|section)\b', ln)
        if m_sec:
            in_rdata = (m_sec.group(1) == "rdata")
        elif in_rdata and re.match(r'^\s*\.align\s+3\b', ln):
            al3_indices.append(i)
    if not al3_indices:
        return s_text

    probe_lines = list(lines)
    for k, idx in enumerate(al3_indices):
        probe_lines[idx] = f"\t.align\t2\n\t.globl\t__wo64_al3_{k}\n__wo64_al3_{k}:\n"
    probe_elf = assemble_fn("".join(probe_lines))
    _, syms = elf_section_size_and_syms(probe_elf, "__wo64_al3_")

    extra = 0
    final_lines = list(lines)
    for k, idx in enumerate(al3_indices):
        v = syms[f"__wo64_al3_{k}"]
        if (ro_len + v + extra) % 8 != 0:
            final_lines[idx] = "\t.align\t2\n\t.word\t0\n"
            extra += 4
        else:
            final_lines[idx] = "\t.align\t2\n"
    return "".join(final_lines)


def assemble_stub_bytes(asm_path, version, ro_len=0):
    asm_path = Path(asm_path)
    asm_text = asm_path.read_text(encoding="utf-8")
    if ".section" not in asm_text and "\n.text" not in asm_text:
        asm_text = '.include "macro.inc"\n.set noat\n.set noreorder\n.set gp=64\n.section .text, "ax"\n' + asm_text
    for ro_path in (
        asm_path.with_suffix(".rodata.s"),
        ROOT / "config" / "rodata_stubs" / version / f"{asm_path.stem}.rodata.s",
    ):
        if ro_path.is_file():
            ro_text = ro_path.read_text(encoding="utf-8")
            ro_text = re.sub(r'^\s*\.include\s+"macro\.inc"\s*$', "", ro_text, flags=re.M)
            if ro_len % 8 == 4 and re.search(r'^\s*\.section\s+\.rodata\s*\n\s*\.balign\s+8\b', ro_text, re.M):
                ro_text = re.sub(
                    r'(^\s*\.section\s+\.rodata\s*\n)\s*\.balign\s+8\b',
                    r'\1.word 0',
                    ro_text,
                    count=1,
                    flags=re.M,
                )
            asm_text = asm_text + "\n" + ro_text + "\n"
            break

    if not use_docker_binutils():
        bu = resolve_native_binutils()
        with tempfile.TemporaryDirectory(prefix="w64as_", dir=_short_tmp_base()) as tmp:
            tp = Path(tmp)
            in_s = tp / "in.s"
            out_o = tp / "out.o"
            in_s.write_text(asm_text, encoding="utf-8", newline="\n")
            run_host([
                bu["as"], "-EB", "-march=vr4300", "-mtune=vr4300", "-mabi=32",
                "-no-pad-sections", "-G0", f"-I{ROOT / 'include'}",
                "-o", str(out_o), str(in_s),
            ])
            return out_o.read_bytes()

    return docker_exec(
        'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
        'cat > "$d/in.s" && '
        f'{BINUTILS_PREFIX}as -EB -march=vr4300 -mtune=vr4300 -mabi=32 '
        '-no-pad-sections -G0 -I/work/include -o "$d/out.o" "$d/in.s" && '
        'cat "$d/out.o"',
        input_bytes=asm_text.encode("utf-8"),
    )


def strip_empty_dummy_and_split_s(s_text, dummy_markers):
    """Split cc1n64 assembly output around dummy marker functions into ordered chunks."""
    lines = s_text.splitlines(keepends=True)
    chunks = []
    cur = []
    in_dummy = None
    for ln in lines:
        m_ent = re.match(r'^\s*\.ent\s+(\S+)', ln)
        if m_ent and m_ent.group(1) in dummy_markers:
            while cur and (
                re.match(r'^\s*\.globl\s+' + re.escape(m_ent.group(1)) + r'\b', cur[-1])
                or re.match(r'^\s*\.align\b', cur[-1])
                or cur[-1].strip() == ""
            ):
                cur.pop()
            if cur:
                chunks.append(("c_s", "".join(cur)))
                cur = []
            in_dummy = m_ent.group(1)
            chunks.append(("stub", in_dummy))
            continue
        if in_dummy is not None:
            m_end = re.match(r'^\s*\.end\s+(\S+)', ln)
            if m_end and m_end.group(1) == in_dummy:
                in_dummy = None
            continue
        cur.append(ln)
    if cur:
        chunks.append(("c_s", "".join(cur)))
    return chunks


def _merge_part_objs(part_objs):
    """Relocatably link part .o bytes in order, enforcing 4-byte section alignment and localizing __wo64_lc_*."""
    if not use_docker_binutils():
        bu = resolve_native_binutils()
        with tempfile.TemporaryDirectory(prefix="w64ld_", dir=_short_tmp_base()) as tmp:
            tp = Path(tmp)
            names = []
            for i, b in enumerate(part_objs):
                p = tp / f"part_{i:04d}.o"
                pa = tp / f"part_{i:04d}.a.o"
                p.write_bytes(b)
                run_host([
                    bu["objcopy"],
                    "--set-section-alignment", ".text=4",
                    "--set-section-alignment", ".rodata=4",
                    str(p), str(pa),
                ])
                names.append(str(pa))
            merged = tp / "merged.o"
            merged2 = tp / "merged2.o"
            run_host([bu["ld"], "-EB", "-r", "-o", str(merged), *names])
            run_host([bu["objcopy"], "-w", "-L", "__wo64_lc_*", str(merged), str(merged2)])
            return merged2.read_bytes()

    tar_buf = io.BytesIO()
    names = []
    with tarfile.open(fileobj=tar_buf, mode="w") as tf:
        for i, b in enumerate(part_objs):
            nm = f"part_{i:04d}.o"
            names.append(nm)
            ti = tarfile.TarInfo(nm)
            ti.size = len(b)
            tf.addfile(ti, io.BytesIO(b))
    args_str = " ".join(shlex.quote(n) for n in names)
    return docker_exec(
        f'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
        f'tar -xf - -C "$d" && cd "$d" && '
        f'for f in {args_str}; do '
        f'  {BINUTILS_PREFIX}objcopy --set-section-alignment .text=4 --set-section-alignment .rodata=4 "$f" "$f.a" && mv "$f.a" "$f"; '
        f'done && '
        f'{BINUTILS_PREFIX}ld -EB -r -o merged.o {args_str} && '
        f'{BINUTILS_PREFIX}objcopy -w -L "__wo64_lc_*" merged.o merged2.o && '
        f'cat merged2.o',
        input_bytes=tar_buf.getvalue(),
    )


def compile_with_include_asm(src_path, out_path, version, opt, extra_cc1, extra_cpp, save_asm=False):
    raw_text = src_path.read_text(encoding="utf-8")
    if "INCLUDE_ASM" not in raw_text:
        elf_bytes = compile_c_to_obj_bytes(
            raw_text,
            src_path.parent,
            version,
            opt,
            extra_cc1,
            extra_cpp,
            save_prefix=str(out_path) if save_asm else None,
        )
        out_path.parent.mkdir(parents=True, exist_ok=True)
        out_path.write_bytes(elf_bytes)
        return

    ver_def = "-DVERSION_US" if version == "us" else "-DVERSION_EU"
    with tempfile.TemporaryDirectory(prefix="w64m_", dir=_short_tmp_base()) as tmp:
        tmp_p = Path(tmp)
        c_path = tmp_p / "in.c"
        i_raw_path = tmp_p / "in_raw.i"
        i_path = tmp_p / "in.i"
        s_path = tmp_p / "out.s"
        c_path.write_text(raw_text, encoding="utf-8")

        libreultra_inc = TOOLCHAIN / "libreultra" / "include" / "2.0I"
        extra_inc = [f"-I{libreultra_inc}"] if libreultra_inc.is_dir() else []
        run_host(win32_cmd(
            CPP, "-lang-c", "-undef",
            "-D__GNUC__=2", "-D_MIPS_SZLONG=32", "-D_MIPS_SZINT=32",
            "-D_LANGUAGE_C", "-DLANGUAGE_C", "-Dmips", "-D__mips__",
            "-DMIPSEB", "-D_MIPSEB", "-DF3DEX_GBI", ver_def,
            "-DINCLUDE_ASM(f,n)=__WO64_INCLUDE_ASM_CALL__(f,n)",
            f"-I{ROOT / 'include'}", *extra_inc, f"-I{src_path.parent}",
            *extra_cpp, str(c_path), str(i_raw_path),
        ))

        pre_text = i_raw_path.read_text(encoding="utf-8")
        stubs = {}
        idx = 0

        def _repl_stub(m):
            nonlocal idx
            folder, func_name = m.group(1), m.group(2)
            marker = f"__WO64_INCLUDE_ASM_{idx}__"
            stub_p = ROOT / "asm" / version / folder / f"{func_name}.s"
            if not stub_p.is_file():
                raise FileNotFoundError(f"INCLUDE_ASM target not found: {stub_p}")
            stubs[marker] = stub_p
            idx += 1
            return f"void {marker}(void) {{}}"

        rewritten_i = INCLUDE_ASM_CPP_RE.sub(_repl_stub, pre_text)
        if not stubs:
            elf_bytes = compile_c_to_obj_bytes(
                raw_text,
                src_path.parent,
                version,
                opt,
                extra_cc1,
                extra_cpp,
                save_prefix=str(out_path) if save_asm else None,
            )
            out_path.parent.mkdir(parents=True, exist_ok=True)
            out_path.write_bytes(elf_bytes)
            return

        i_path.write_text(rewritten_i, encoding="utf-8")
        run_host(win32_cmd(
            CC1, "-quiet", opt, "-G0",
            "-mips3", "-mcpu=4300", "-mgas", "-meb",
            *extra_cc1, str(i_path), "-o", str(s_path),
        ))
        s_text = s_path.read_text(encoding="utf-8")
        if save_asm:
            Path(f"{out_path}.s").write_text(s_text, encoding="utf-8")

        lc_defs = set(re.findall(r'^(\$LC\d+):', s_text, re.M))
        chunks = strip_empty_dummy_and_split_s(s_text, stubs)
        if len(chunks) > 1 and lc_defs:
            def_chunk = {}
            for ci, (kind, payload) in enumerate(chunks):
                if kind == "c_s":
                    for lbl in re.findall(r'^(\$LC\d+):', payload, re.M):
                        def_chunk[lbl] = ci
            cross_lc = set()
            for ci, (kind, payload) in enumerate(chunks):
                if kind == "c_s":
                    for lbl in re.findall(r'\$LC\d+', payload):
                        if lbl in def_chunk and def_chunk[lbl] != ci:
                            cross_lc.add(lbl)
            if cross_lc:
                new_chunks = []
                for ci, (kind, payload) in enumerate(chunks):
                    if kind == "c_s":
                        for lbl in cross_lc:
                            glbl = f"__wo64_lc_{lbl[3:]}"
                            payload = re.sub(
                                rf'^{re.escape(lbl)}:',
                                f'\t.globl\t{glbl}\n{glbl}:',
                                payload,
                                flags=re.M,
                            )
                            payload = re.sub(rf'{re.escape(lbl)}\b', glbl, payload)
                    new_chunks.append((kind, payload))
                chunks = new_chunks

        part_objs = []
        ro_len = 0

        def _assemble_c_s(ci_tag, asm_str):
            ps = tmp_p / f"p{ci_tag}.s"
            po = tmp_p / f"p{ci_tag}.obj"
            ps.write_text(asm_str, encoding="utf-8", newline="\r\n")
            run_host(win32_cmd(ASN, "-q", "-G0", "-o", str(po), str(ps)))
            return lnk_to_elf_bytes(po.read_bytes())

        for ci, (kind, payload) in enumerate(chunks):
            ro_start = (ro_len + 3) & ~3
            if kind == "c_s":
                if not any(
                    l.strip() and not l.strip().startswith("#")
                    for l in payload.splitlines()
                ):
                    continue
                prefix = "\t.text\n" if (ci > 0 and not re.match(r'^\s*\.(text|rdata|data|sdata|bss|comm|lcomm)\b', payload.lstrip())) else ""
                full_s = _fix_rdata_align3_in_s(
                    prefix + payload,
                    ro_start,
                    lambda s_probe, _ci=ci: _assemble_c_s(f"{_ci}_probe", s_probe),
                )
                elf_b = _assemble_c_s(str(ci), full_s)
                ro_sz, _ = elf_section_size_and_syms(elf_b)
                if ro_sz:
                    ro_len = ro_start + ro_sz
                part_objs.append(elf_b)
            else:
                elf_b = assemble_stub_bytes(stubs[payload], version, ro_len=ro_start)
                ro_sz, _ = elf_section_size_and_syms(elf_b)
                if ro_sz:
                    ro_len = ro_start + ro_sz
                part_objs.append(elf_b)

    merged = _merge_part_objs(part_objs)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_bytes(merged)


def main():
    ap = argparse.ArgumentParser(description="Compile Wipeout 64 C source via SN64 toolchain")
    ap.add_argument("input", type=Path, help="Input .c file")
    ap.add_argument("output", type=Path, help="Output .o file")
    ap.add_argument("--version", choices=["us", "eu"], default=os.environ.get("VERSION", "us"))
    ap.add_argument("--opt", default=os.environ.get("CC_OPT", "-O2"))
    ap.add_argument("--save-asm", action="store_true", default=os.environ.get("CC_SAVE_ASM") == "1")
    ap.add_argument("--cpp-flag", action="append", default=[])
    ap.add_argument("--cc1-flag", action="append", default=[])
    args = ap.parse_args()

    compile_with_include_asm(
        args.input.resolve(),
        args.output.resolve(),
        args.version,
        args.opt,
        args.cc1_flag,
        args.cpp_flag,
        save_asm=args.save_asm,
    )


if __name__ == "__main__":
    main()
