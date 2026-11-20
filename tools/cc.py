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


def assemble_stub_bytes(asm_path):
    asm_bytes = Path(asm_path).read_bytes()
    return docker_exec(
        'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
        'cat > "$d/in.s" && '
        'mips-linux-gnu-as -EB -march=vr4300 -mtune=vr4300 -mabi=32 '
        '-no-pad-sections -G0 -I/work/include -o "$d/out.o" "$d/in.s" && '
        'cat "$d/out.o"',
        input_bytes=asm_bytes,
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


def compile_with_include_asm(src_path, out_path, version, opt, extra_cc1, extra_cpp, save_asm=False):
    raw_lines = src_path.read_text(encoding="utf-8").splitlines(keepends=True)
    stubs = {}
    rewritten = []
    idx = 0
    for ln in raw_lines:
        m = INCLUDE_ASM_RE.match(ln)
        if m:
            folder, func_name = m.group(1), m.group(2)
            marker = f"__WO64_INCLUDE_ASM_{idx}__"
            stubs[marker] = ROOT / "asm" / version / folder / f"{func_name}.s"
            if not stubs[marker].is_file():
                raise FileNotFoundError(f"INCLUDE_ASM target not found: {stubs[marker]}")
            rewritten.append(f"void {marker}(void) {{}}\n")
            idx += 1
        else:
            rewritten.append(ln)

    if not stubs:
        elf_bytes = compile_c_to_obj_bytes(
            "".join(raw_lines),
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

    # Compile rewritten C once through cppn64 + cc1n64, then split assembly around markers
    ver_def = "-DVERSION_US" if version == "us" else "-DVERSION_EU"
    with tempfile.TemporaryDirectory(prefix="wo64mix_") as tmp:
        tmp_p = Path(tmp)
        c_path = tmp_p / "in.c"
        i_path = tmp_p / "in.i"
        s_path = tmp_p / "out.s"
        c_path.write_text("".join(rewritten), encoding="utf-8")

        libreultra_inc = TOOLCHAIN / "libreultra" / "include" / "2.0I"
        extra_inc = [f"-I{libreultra_inc}"] if libreultra_inc.is_dir() else []
        run_host([
            str(WIBO), str(CPP), "-lang-c", "-undef",
            "-D__GNUC__=2", "-D_MIPS_SZLONG=32", "-D_MIPS_SZINT=32",
            "-D_LANGUAGE_C", "-DLANGUAGE_C", "-Dmips", "-D__mips__",
            "-DMIPSEB", "-D_MIPSEB", "-DF3DEX_GBI", ver_def,
            f"-I{ROOT / 'include'}", *extra_inc, f"-I{src_path.parent}",
            *extra_cpp, str(c_path), str(i_path),
        ])
        run_host([
            str(WIBO), str(CC1), "-quiet", opt, "-G0",
            "-mips3", "-mcpu=4300", "-mgas", "-meb",
            *extra_cc1, str(i_path), "-o", str(s_path),
        ])
        s_text = s_path.read_text(encoding="utf-8")
        if save_asm:
            Path(f"{out_path}.s").write_text(s_text, encoding="utf-8")

        chunks = strip_empty_dummy_and_split_s(s_text, stubs)
        # Assemble each chunk in order, then ld -r in container
        part_objs = []
        for ci, (kind, payload) in enumerate(chunks):
            if kind == "c_s":
                # Skip chunks that have no directives/instructions besides blank lines/comments
                if not any(
                    l.strip() and not l.strip().startswith("#")
                    for l in payload.splitlines()
                ):
                    continue
                ps = tmp_p / f"p{ci}.s"
                po = tmp_p / f"p{ci}.obj"
                # Ensure .text is active if chunk starts with a label/func after a split
                prefix = "\t.text\n" if (ci > 0 and not re.match(r'^\s*\.(text|rdata|data|sdata|bss|comm|lcomm)\b', payload.lstrip())) else ""
                ps.write_text(prefix + payload, encoding="utf-8")
                run_host([str(WIBO), str(ASN), "-q", "-G0", "-o", str(po), str(ps)])
                elf_b = docker_exec(
                    f'd=$(mktemp -d) && trap "rm -rf $d" EXIT && '
                    f'cat > "$d/in.obj" && '
                    f'{POP_IN_CONTAINER} "$d/in.obj" -b -o "$d/out.o" >/dev/null && '
                    f'mips-linux-gnu-objcopy --rename-section .rdata=.rodata "$d/out.o" "$d/out2.o" && '
                    f'cat "$d/out2.o"',
                    input_bytes=po.read_bytes(),
                )
                part_objs.append(elf_b)
            else:
                part_objs.append(assemble_stub_bytes(stubs[payload]))

    # Relocatably link all part_objs in exact order inside container
    # Pass via tar stream on stdin for zero host-mount staleness issues
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
        f'mips-linux-gnu-ld -EB -r -o merged.o {args_str} && '
        f'cat merged.o',
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
