#!/usr/bin/env python3
"""Cross-platform build & byte-verification driver for Wipeout 64 (USA and Europe).

Works identically on Linux (x86_64/arm64, native binutils or Docker),
macOS (Apple Silicon and Intel x64), and Windows (native SN64 + Docker Desktop or native binutils).
"""
import argparse
import hashlib
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(__file__).resolve().parent))
import cc


def _needs_rebuild(out_path, deps):
    if not out_path.is_file():
        return True
    out_m = out_path.stat().st_mtime
    return any(d.is_file() and d.stat().st_mtime > out_m for d in deps)


def _gather_asm_and_bin(ver):
    asm_dir = ROOT / "asm" / ver
    asm_files = []
    if asm_dir.is_dir():
        for p in sorted(asm_dir.rglob("*.s")):
            rel = p.relative_to(asm_dir)
            if rel.parts and rel.parts[0] in ("nonmatchings", "matchings"):
                continue
            # Match original maxdepth 3 behavior under asm/<ver>
            if len(rel.parts) <= 3:
                asm_files.append(p)

    assets_dir = ROOT / "assets" / ver
    bin_files = sorted(assets_dir.rglob("*.bin")) if assets_dir.is_dir() else []
    return asm_files, bin_files


def _assemble_and_link_native(ver):
    bu = cc.resolve_native_binutils()
    asm_files, bin_files = _gather_asm_and_bin(ver)

    for f in asm_files:
        rel_posix = f.relative_to(ROOT).as_posix()
        out = ROOT / "build" / ver / f"{rel_posix}.o"
        if _needs_rebuild(out, [f]):
            out.parent.mkdir(parents=True, exist_ok=True)
            cc.run_host([
                bu["as"], "-EB", "-march=vr4300", "-mtune=vr4300", "-mabi=32",
                "-no-pad-sections", "-G0", f"-I{ROOT / 'include'}",
                "-o", str(out), str(f),
            ], cwd=ROOT)

    for f in bin_files:
        rel_posix = f.relative_to(ROOT).as_posix()
        out = ROOT / "build" / ver / f"{rel_posix}.o"
        if _needs_rebuild(out, [f]):
            out.parent.mkdir(parents=True, exist_ok=True)
            cc.run_host([
                bu["ld"], "-EB", "-r", "-b", "binary",
                "-o", str(out), rel_posix,
            ], cwd=ROOT)

    elf_out = ROOT / "build" / ver / "wipeout64.elf"
    map_out = ROOT / "build" / ver / "wipeout64.map"
    rom_out = ROOT / "build" / ver / "wipeout64.z64"
    cc.run_host([
        bu["ld"], "-EB", "--no-check-sections", "-nostdlib",
        "-T", "config/common_syms.ld",
        "-T", f"config/{ver}/syms.ld",
        "-T", f"build/{ver}/wipeout64.ld",
        "-T", f"build/{ver}/undefined_funcs_auto.txt",
        "-T", f"build/{ver}/undefined_syms_auto.txt",
        "-Map", str(map_out),
        "-o", str(elf_out),
    ], cwd=ROOT)
    cc.run_host([bu["objcopy"], "-O", "binary", str(elf_out), str(rom_out)], cwd=ROOT)


def _assemble_and_link_docker(ver):
    prefix = cc.BINUTILS_PREFIX
    cmd = (
        f"VER='{ver}'\n"
        f"AS='{prefix}as -EB -march=vr4300 -mtune=vr4300 -mabi=32 -no-pad-sections -G0 -Iinclude'\n"
        f"LD='{prefix}ld -EB --no-check-sections -nostdlib'\n"
        f"OBJCOPY='{prefix}objcopy'\n"
        'for f in $(find "asm/$VER" -maxdepth 3 -name "*.s" '
        '! -path "asm/$VER/nonmatchings/*" ! -path "asm/$VER/matchings/*" | sort); do\n'
        '    out="build/$VER/${f}.o"\n'
        '    if [ ! -f "$out" ] || [ "$f" -nt "$out" ]; then\n'
        '        mkdir -p "$(dirname "$out")"\n'
        '        $AS -o "$out" "$f"\n'
        '    fi\n'
        'done\n'
        'for f in $(find "assets/$VER" -name "*.bin" | sort); do\n'
        '    out="build/$VER/${f}.o"\n'
        '    if [ ! -f "$out" ] || [ "$f" -nt "$out" ]; then\n'
        '        mkdir -p "$(dirname "$out")"\n'
        '        $LD -r -b binary -o "$out" "$f"\n'
        '    fi\n'
        'done\n'
        '$LD -T config/common_syms.ld '
        '-T "config/$VER/syms.ld" '
        '-T "build/$VER/wipeout64.ld" '
        '-T "build/$VER/undefined_funcs_auto.txt" '
        '-T "build/$VER/undefined_syms_auto.txt" '
        '-Map "build/$VER/wipeout64.map" '
        '-o "build/$VER/wipeout64.elf"\n'
        '$OBJCOPY -O binary "build/$VER/wipeout64.elf" "build/$VER/wipeout64.z64"\n'
    )
    cc.docker_exec(cmd)


def build_version(ver):
    baserom = ROOT / "orig" / ver / "baserom.z64"
    if not baserom.is_file():
        raise SystemExit(f"Missing {baserom.relative_to(ROOT)}")

    cfg = ROOT / "config" / ver / "wipeout64.yaml"
    sym_addrs = ROOT / "config" / ver / "symbol_addrs.txt"
    ld_script = ROOT / "build" / ver / "wipeout64.ld"

    if _needs_rebuild(ld_script, [cfg, sym_addrs]):
        print(f"=== Splitting {ver} ===", flush=True)
        # Pass relative config path so splat emits portable relative .incbin paths
        subprocess.run([sys.executable, "-m", "splat", "split", f"config/{ver}/wipeout64.yaml"], cwd=ROOT, check=True)

    # Compile any C files referenced by the linker script
    ld_text = ld_script.read_text(encoding="utf-8")
    c_objs = sorted(set(re.findall(rf"build/{ver}/(src/[^\s(]+\.c)\.o", ld_text)))
    headers = list((ROOT / "include").rglob("*.h"))
    newest_hdr = max((h.stat().st_mtime for h in headers), default=0.0)

    for src_rel in c_objs:
        src_p = ROOT / src_rel
        out_p = ROOT / "build" / ver / f"{src_rel}.o"
        if not src_p.is_file():
            raise SystemExit(f"Missing C source referenced by linker script: {src_rel}")
        need = not out_p.is_file()
        if not need:
            out_m = out_p.stat().st_mtime
            if src_p.stat().st_mtime > out_m or newest_hdr > out_m:
                need = True
        if need:
            print(f"CC [{ver}] {src_rel}")
            cc.compile_with_include_asm(src_p.resolve(), out_p.resolve(), ver, "-O2", [], [])

    if cc.use_docker_binutils():
        _assemble_and_link_docker(ver)
    else:
        _assemble_and_link_native(ver)

    built_rom = ROOT / "build" / ver / "wipeout64.z64"
    orig_bytes = baserom.read_bytes()
    built_bytes = built_rom.read_bytes()
    if orig_bytes == built_bytes:
        sha = hashlib.sha1(built_bytes).hexdigest()
        print(f"OK {ver}: {sha} (byte-identical)")
    else:
        first_diff = next(
            (i for i in range(min(len(orig_bytes), len(built_bytes))) if orig_bytes[i] != built_bytes[i]),
            min(len(orig_bytes), len(built_bytes)),
        )
        print(
            f"MISMATCH {ver}: {built_rom.relative_to(ROOT)} differs from {baserom.relative_to(ROOT)} "
            f"(len orig={len(orig_bytes)}, built={len(built_bytes)}, first diff at 0x{first_diff:x})",
            file=sys.stderr,
        )
        raise SystemExit(1)


def main():
    ap = argparse.ArgumentParser(description="Build and verify Wipeout 64 ROM(s)")
    ap.add_argument("target", nargs="?", default="all", choices=["us", "eu", "all"])
    args = ap.parse_args()

    versions = ["us", "eu"] if args.target == "all" else [args.target]
    for ver in versions:
        build_version(ver)


if __name__ == "__main__":
    main()
