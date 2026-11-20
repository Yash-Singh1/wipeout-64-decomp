#!/usr/bin/env python3
"""Compile a Wipeout 64 C source file to a big-endian MIPS ELF object.

Pipeline:
  cppn64.exe (via wibo-macos)
    -> cc1n64.exe (SN32.3.7.0006, -O2 -G0 -mips3 -mcpu=4300 -mgas -meb)
    -> asn64.exe  (SN ASN64 2.72, -q -G0)
    -> psyq-obj-parser -b + mips-linux-gnu-objcopy --rename-section .rdata=.rodata
       (inside long-lived Docker container wipeout64-cc using wipeoutxl:latest)

Supports INCLUDE_ASM("nonmatchings/<tu>", <func>) inside .c files: C chunks are
compiled with SN64, referenced .s stubs are assembled with mips-linux-gnu-as,
and the pieces are combined in exact source order with mips-linux-gnu-ld -r.
"""
import argparse
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TOOLCHAIN = Path(os.environ.get("WIPEOUT64_TOOLCHAIN", ROOT / "toolchains"))
WIBO = TOOLCHAIN / "bin" / "wibo-macos"
SN64 = TOOLCHAIN / "sn272_0006"
CPP = SN64 / "cppn64.exe"
CC1 = SN64 / "cc1n64.exe"
ASN = SN64 / "asn64.exe"
POP_IN_CONTAINER = "/work/toolchains/bin/psyq-obj-parser"
DOCKER_IMAGE = os.environ.get("WIPEOUT64_IMAGE", "wipeoutxl:latest")
CONTAINER_NAME = "wipeout64-cc"

INCLUDE_ASM_RE = re.compile(
    r'^\s*INCLUDE_ASM\s*\(\s*"([^"]+)"\s*,\s*([A-Za-z0-9_.$]+)\s*\)\s*;?\s*$'
)


def ensure_container():
    res = subprocess.run(
        ["docker", "inspect", "-f", "{{.State.Running}}", CONTAINER_NAME],
        capture_output=True,
        text=True,
    )
    if res.returncode == 0 and res.stdout.strip() == "true":
        return
    # Remove any stopped container with the same name
    subprocess.run(["docker", "rm", "-f", CONTAINER_NAME], capture_output=True)
    r = subprocess.run(
        [
            "docker",
            "run",
            "-d",
            "--rm",
            "--name",
            CONTAINER_NAME,
            "-v",
            f"{ROOT}:/work",
            "-w",
            "/work",
            DOCKER_IMAGE,
            "sleep",
            "infinity",
        ],
        capture_output=True,
        text=True,
    )
    if r.returncode != 0:
        raise RuntimeError(f"Failed to start container {CONTAINER_NAME}: {r.stderr}")


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


def run_host(args, cwd=None):
    r = subprocess.run(args, cwd=cwd, capture_output=True, text=True)
    if r.returncode != 0:
        msg = (r.stdout or "") + (r.stderr or "")
        raise RuntimeError(f"Command failed: {' '.join(map(str, args))}\n{msg}")
    return r


def compile_c_to_obj_bytes(src_text, src_dir, version, opt, extra_cc1, extra_cpp, save_prefix=None):
    """Compile C source text (without INCLUDE_ASM) -> big-endian ELF .o bytes."""
    ver_def = "-DVERSION_US" if version == "us" else "-DVERSION_EU"
    with tempfile.TemporaryDirectory(prefix="wo64cc_") as tmp:
        tmp_p = Path(tmp)
        c_path = tmp_p / "in.c"
        i_path = tmp_p / "in.i"
        s_path = tmp_p / "out.s"
        obj_path = tmp_p / "out.obj"
        c_path.write_text(src_text, encoding="utf-8")

        libreultra_inc = TOOLCHAIN / "libreultra" / "include" / "2.0I"
        extra_inc = [f"-I{libreultra_inc}"] if libreultra_inc.is_dir() else []
        cpp_cmd = [
            str(WIBO),
            str(CPP),
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
        ]
        run_host(cpp_cmd)

        cc1_cmd = [
            str(WIBO),
            str(CC1),
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
        ]
        run_host(cc1_cmd)

        if save_prefix:
            shutil.copyfile(s_path, f"{save_prefix}.s")

        asn_cmd = [
            str(WIBO),
            str(ASN),
            "-q",
            "-G0",
            "-o",
            str(obj_path),
            str(s_path),
        ]
        run_host(asn_cmd)

        lnk_bytes = obj_path.read_bytes()

    # Pipe LNK object into container -> psyq-obj-parser -b -> objcopy .rdata->.rodata -> stdout
    elf_bytes = docker_exec(
        f'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
        f'cat > "$d/in.obj" && '
        f'{POP_IN_CONTAINER} "$d/in.obj" -b -o "$d/out.o" >/dev/null && '
        f'mips-linux-gnu-objcopy --rename-section .rdata=.rodata "$d/out.o" "$d/out2.o" && '
        f'cat "$d/out2.o"',
        input_bytes=lnk_bytes,
    )
    return elf_bytes


def elf_section_size_and_syms(elf_bytes, sym_prefix=None):
    """Return (rodata_size, {name: st_value}) from a big-endian 32-bit ELF object."""
    import struct
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

    # Find all .align 3 lines that occur while inside .rdata
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

    # Probe pass: replace each .align 3 with .align 2 + marker label
    probe_lines = list(lines)
    for k, idx in enumerate(al3_indices):
        probe_lines[idx] = f"\t.align\t2\n\t.globl\t__wo64_al3_{k}\n__wo64_al3_{k}:\n"
    probe_elf = assemble_fn("".join(probe_lines))
    _, syms = elf_section_size_and_syms(probe_elf, "__wo64_al3_")

    # Compute exact padding needed at each .align 3 for true offset (ro_len + v + extra) % 8 == 0
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
    # Ensure .text is active (spimdisasm nonmatchings/*.s files omit .section .text)
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
    return docker_exec(
        'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
        'cat > "$d/in.s" && '
        'mips-linux-gnu-as -EB -march=vr4300 -mtune=vr4300 -mabi=32 '
        '-no-pad-sections -G0 -I/work/include -o "$d/out.o" "$d/in.s" && '
        'cat "$d/out.o"',
        input_bytes=asm_text.encode("utf-8"),
    )


def strip_empty_dummy_and_split_s(s_text, dummy_markers):
    """Split cc1n64 assembly output around dummy marker functions into ordered chunks.

    Returns (header_and_rodata, {marker_name: None}, list of (kind, payload)) where
    kind is 'c_s' (assembly text to run through asn64) or 'stub' (marker_name).
    """
    lines = s_text.splitlines(keepends=True)
    chunks = []
    cur = []
    in_dummy = None
    for ln in lines:
        m_ent = re.match(r'^\s*\.ent\s+(\S+)', ln)
        if m_ent and m_ent.group(1) in dummy_markers:
            # Drop preceding .globl/.align lines belonging to this dummy
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


INCLUDE_ASM_CPP_RE = re.compile(
    r'__WO64_INCLUDE_ASM_CALL__\s*\(\s*"([^"]+)"\s*,\s*([A-Za-z0-9_.$]+)\s*\)\s*;?'
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

    # Preprocess first so #ifdef VERSION_EU / #ifdef NON_MATCHING select active INCLUDE_ASM stubs
    ver_def = "-DVERSION_US" if version == "us" else "-DVERSION_EU"
    with tempfile.TemporaryDirectory(prefix="wo64mix_") as tmp:
        tmp_p = Path(tmp)
        c_path = tmp_p / "in.c"
        i_raw_path = tmp_p / "in_raw.i"
        i_path = tmp_p / "in.i"
        s_path = tmp_p / "out.s"
        c_path.write_text(raw_text, encoding="utf-8")

        libreultra_inc = TOOLCHAIN / "libreultra" / "include" / "2.0I"
        extra_inc = [f"-I{libreultra_inc}"] if libreultra_inc.is_dir() else []
        run_host([
            str(WIBO), str(CPP), "-lang-c", "-undef",
            "-D__GNUC__=2", "-D_MIPS_SZLONG=32", "-D_MIPS_SZINT=32",
            "-D_LANGUAGE_C", "-DLANGUAGE_C", "-Dmips", "-D__mips__",
            "-DMIPSEB", "-D_MIPSEB", "-DF3DEX_GBI", ver_def,
            "-DINCLUDE_ASM(f,n)=__WO64_INCLUDE_ASM_CALL__(f,n)",
            f"-I{ROOT / 'include'}", *extra_inc, f"-I{src_path.parent}",
            *extra_cpp, str(c_path), str(i_raw_path),
        ])

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
        run_host([
            str(WIBO), str(CC1), "-quiet", opt, "-G0",
            "-mips3", "-mcpu=4300", "-mgas", "-meb",
            *extra_cc1, str(i_path), "-o", str(s_path),
        ])
        s_text = s_path.read_text(encoding="utf-8")
        if save_asm:
            Path(f"{out_path}.s").write_text(s_text, encoding="utf-8")

        # Promote $LC<N> labels so string-pooled .rodata references resolve across split chunks
        lc_defs = set(re.findall(r'^(\$LC\d+):', s_text, re.M))
        chunks = strip_empty_dummy_and_split_s(s_text, stubs)
        if len(chunks) > 1 and lc_defs:
            # Find which $LC labels are referenced in a different chunk than defined
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

        # Assemble each chunk in order (tracking cumulative .rodata offset for .align 3 phase),
        # then ld -r in container
        part_objs = []
        ro_len = 0

        def _assemble_c_s(ci_tag, asm_str):
            ps = tmp_p / f"p{ci_tag}.s"
            po = tmp_p / f"p{ci_tag}.obj"
            ps.write_text(asm_str, encoding="utf-8", newline="\r\n")
            run_host([str(WIBO), str(ASN), "-q", "-G0", "-o", str(po), str(ps)])
            return docker_exec(
                f'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
                f'cat > "$d/in.obj" && '
                f'{POP_IN_CONTAINER} "$d/in.obj" -b -o "$d/out.o" >/dev/null && '
                f'mips-linux-gnu-objcopy --rename-section .rdata=.rodata "$d/out.o" "$d/out2.o" && '
                f'cat "$d/out2.o"',
                input_bytes=po.read_bytes(),
            )

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

    # Relocatably link all part_objs in exact order inside container and localize __wo64_lc_*
    import io, tarfile
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
    merged = docker_exec(
        f'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
        f'tar -xf - -C "$d" && cd "$d" && '
        f'for f in {args_str}; do '
        f'  mips-linux-gnu-objcopy --set-section-alignment .text=4 --set-section-alignment .rodata=4 "$f" "$f.a" && mv "$f.a" "$f"; '
        f'done && '
        f'mips-linux-gnu-ld -EB -r -o merged.o {args_str} && '
        f'mips-linux-gnu-objcopy -w -L "__wo64_lc_*" merged.o merged2.o && '
        f'cat merged2.o',
        input_bytes=tar_buf.getvalue(),
    )
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
