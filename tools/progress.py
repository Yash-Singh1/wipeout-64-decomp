#!/usr/bin/env python3
"""Report Wipeout 64 decompilation progress for USA and Europe."""
import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def count_version(ver):
    ld_path = ROOT / "build" / ver / "wipeout64.ld"
    asm_dir = ROOT / "asm" / ver
    if not ld_path.is_file() or not asm_dir.is_dir():
        print(f"[{ver}] Not split/built yet. Run ./build.sh {ver} first.")
        return

    ld_text = ld_path.read_text(encoding="utf-8")
    # Measure game code vs libultra bytes from splat asm + linked C objects
    c_objs = sorted(set(re.findall(rf"build/{ver}/(src/[^\s(]+\.c)\.o", ld_text)))
    c_funcs = 0
    c_bytes = 0
    asm_funcs = 0
    asm_bytes = 0
    lib_funcs = 0
    lib_bytes = 0

    def scan_s(p):
        nf = nb = 0
        cur = None
        for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = re.match(r"^glabel\s+(\S+)", ln)
            if m:
                cur = [None, None]
            elif cur is not None:
                m2 = re.match(r"^\s*/\*\s*([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s*\*/", ln)
                if m2:
                    r = int(m2.group(1), 16)
                    if cur[0] is None:
                        cur[0] = r
                    cur[1] = r + 4
                elif ln.startswith("endlabel "):
                    if cur[0] is not None:
                        nf += 1
                        nb += cur[1] - cur[0]
                    cur = None
        return nf, nb

    # Count assembly-only game files and library files actually referenced by the linker script
    asm_objs = sorted(set(re.findall(rf"build/{ver}/(asm/{ver}/[^\s(]+\.s)\.o", ld_text)))
    for rel_s in asm_objs:
        p = ROOT / rel_s
        if not p.is_file():
            continue
        rel = p.relative_to(asm_dir).as_posix()
        if rel in ("header.s", "crt0.s") or rel.startswith(("data/", "matchings/", "nonmatchings/")):
            continue
        nf, nb = scan_s(p)
        if any(k in rel for k in ("libultra", "libmus", "libgcc")):
            lib_funcs += nf
            lib_bytes += nb
        else:
            asm_funcs += nf
            asm_bytes += nb

    # Count matched C functions in linked src/*.c files from wipeout64.map
    map_p = ROOT / "build" / ver / "wipeout64.map"
    if map_p.is_file() and c_objs:
        map_text = map_p.read_text(encoding="utf-8", errors="ignore")
        # Find sections belonging to build/<ver>/src/*.c.o(.text)
        for src_rel in c_objs:
            obj_marker = f"build/{ver}/{src_rel}.o"
            m = re.search(
                rf"^\s*\.text\s+0x[0-9a-fA-F]+\s+(0x[0-9a-fA-F]+)\s+{re.escape(obj_marker)}\s*$",
                map_text,
                re.M,
            )
            if m:
                total_obj_bytes = int(m.group(1), 16)
                # Subtract any active INCLUDE_ASM stub bytes in this file (for this version)
                stub_bytes = 0
                stub_funcs = 0
                for ln in (ROOT / src_rel).read_text(encoding="utf-8").splitlines():
                    mi = re.match(r'^\s*INCLUDE_ASM\s*\(\s*"([^"]+)"\s*,\s*([A-Za-z0-9_.$]+)\s*\)', ln)
                    if mi:
                        sp = asm_dir / mi.group(1) / f"{mi.group(2)}.s"
                        if sp.is_file():
                            snf, snb = scan_s(sp)
                            stub_funcs += snf
                            stub_bytes += snb
                asm_funcs += stub_funcs
                asm_bytes += stub_bytes
                # Count defined function labels in this object inside map
                blk_m = re.search(
                    rf"^\s*\.text\s+0x[0-9a-fA-F]+\s+0x[0-9a-fA-F]+\s+{re.escape(obj_marker)}\s*\n((?:\s+0x[0-9a-fA-F]+\s+\S+\s*\n)*)",
                    map_text,
                    re.M,
                )
                n_syms = 0
                if blk_m:
                    for sln in blk_m.group(1).splitlines():
                        parts = sln.split()
                        if len(parts) >= 2 and not parts[1].startswith(".") and not parts[1].endswith(".NON_MATCHING"):
                            n_syms += 1
                c_funcs += max(0, n_syms - stub_funcs)
                c_bytes += max(0, total_obj_bytes - stub_bytes)

    total_game_bytes = c_bytes + asm_bytes
    total_game_funcs = c_funcs + asm_funcs
    pct = (100.0 * c_bytes / total_game_bytes) if total_game_bytes else 0.0
    print(
        f"[{ver}] Game code: {c_funcs}/{total_game_funcs} funcs, "
        f"{c_bytes}/{total_game_bytes} bytes ({pct:.2f}%) matched in C "
        f"({len(c_objs)} C files linked) | libultra: {lib_funcs} funcs ({lib_bytes} bytes)"
    )


def main():
    ap = argparse.ArgumentParser(description="Wipeout 64 decompilation progress")
    ap.add_argument("--verify", action="store_true", help="Run ./build.sh all before reporting")
    args = ap.parse_args()

    if args.verify:
        subprocess.run([str(ROOT / "build.sh"), "all"], check=True)

    count_version("us")
    count_version("eu")


if __name__ == "__main__":
    main()
