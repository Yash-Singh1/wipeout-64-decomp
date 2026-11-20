#!/usr/bin/env python3
"""Annotated MIPS disassembly of a Wipeout 64 function (USA, Europe, or Prototype)."""
import argparse
import re
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS64, CS_MODE_BIG_ENDIAN

ROOT = Path(__file__).resolve().parents[1]
MAIN_VRAM_BASE = 0x80000400
MAIN_ROM_BASE = 0x1000


def load_rom(ver):
    if ver == "proto":
        p = ROOT / "orig" / "proto" / "Wipeout 64 (Europe) (Proto).n64"
        data = p.read_bytes()
        if data[:4] == b"\x40\x12\x37\x80":
            b = bytearray(len(data))
            b[0::4], b[1::4], b[2::4], b[3::4] = data[3::4], data[2::4], data[1::4], data[0::4]
            data = bytes(b)
        return data
    return (ROOT / "orig" / ver / "baserom.z64").read_bytes()


def load_symbols(ver):
    by_name = {}
    by_addr = {}
    if ver in ("us", "eu"):
        files = [
            ROOT / "config" / "common_syms.ld",
            ROOT / "config" / ver / "symbol_addrs.txt",
            ROOT / "build" / ver / "undefined_funcs_auto.txt",
            ROOT / "build" / ver / "undefined_syms_auto.txt",
        ]
        for p in files:
            if not p.is_file():
                continue
            for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
                m = re.match(r"^\s*([A-Za-z_.$][A-Za-z0-9_.$]*)\s*=\s*(0[xX][0-9A-Fa-f]+|\d+)\s*;", ln)
                if m:
                    name, addr = m.group(1), int(m.group(2), 0)
                    by_name[name] = addr
                    by_addr.setdefault(addr, name)
        # Also read map file for auto-discovered func_XXXXXXXX / D_XXXXXXXX
        map_p = ROOT / "build" / ver / "wipeout64.map"
        if map_p.is_file():
            for ln in map_p.read_text(encoding="utf-8", errors="ignore").splitlines():
                m = re.match(r"^\s+0x00000000([0-9a-fA-F]{8})\s+([A-Za-z_.$][A-Za-z0-9_.$]*)\s*$", ln)
                if m:
                    addr, name = int(m.group(1), 16), m.group(2)
                    by_name.setdefault(name, addr)
                    by_addr.setdefault(addr, name)
    return by_name, by_addr


def find_func_end(rom, start_off, max_bytes=0x4000):
    """Heuristic function end: last jr $ra + delay slot, or hit a known symbol."""
    # Check splat .s files first for exact bounds
    end = start_off + 8
    for off in range(start_off, min(len(rom) - 4, start_off + max_bytes), 4):
        w, = struct.unpack(">I", rom[off:off + 4])
        if w == 0x03E00008:  # jr $ra
            end = off + 8
            # Check if there are branches past this jr $ra from earlier in the function
            # Simple check: look forward 0x20 bytes for another branch target or prologue
            break
    return end


def find_in_splat_asm(ver, target):
    asm_dir = ROOT / "asm" / ver
    if not asm_dir.is_dir():
        return None
    for p in asm_dir.rglob("*.s"):
        text = p.read_text(encoding="utf-8", errors="ignore")
        m = re.search(rf"^glabel\s+{re.escape(target)}\b(.*?)^endlabel\s+{re.escape(target)}\b", text, re.M | re.S)
        if m:
            return p, f"glabel {target}{m.group(1)}endlabel {target}"
    return None


def main():
    ap = argparse.ArgumentParser(description="Disassemble a Wipeout 64 function")
    ap.add_argument("target", help="Function name or hex VRAM address (e.g. NewSkeleton or 0x80019780)")
    ap.add_argument("--version", "-V", choices=["us", "eu", "proto"], default="us")
    ap.add_argument("--size", type=lambda x: int(x, 0), default=None, help="Explicit byte length")
    ap.add_argument("--raw", action="store_true", help="Force raw capstone disasm instead of splat asm")
    args = ap.parse_args()

    by_name, by_addr = load_symbols(args.version)

    if not args.raw and args.version in ("us", "eu") and args.size is None:
        key = args.target
        if re.fullmatch(r"(?:0x)?[0-9A-Fa-f]{8}", key):
            addr = int(key, 16)
            key = by_addr.get(addr, f"func_{addr:08X}")
        hit = find_in_splat_asm(args.version, key)
        if hit:
            p, body = hit
            print(f"; {p.relative_to(ROOT)} ({args.version})")
            print(body)
            return

    if args.target in by_name:
        vram = by_name[args.target]
        label = args.target
    else:
        vram = int(args.target, 16)
        label = by_addr.get(vram, f"func_{vram:08X}")

    rom = load_rom(args.version)
    off = vram - MAIN_VRAM_BASE + MAIN_ROM_BASE
    size = args.size if args.size is not None else (find_func_end(rom, off) - off)

    md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS64 | CS_MODE_BIG_ENDIAN)
    print(f"; {label} ({args.version}) VRAM {vram:#010x} ROM {off:#08x} size {size:#x}")
    for ins in md.disasm(rom[off:off + size], vram):
        comment = ""
        if ins.mnemonic in ("jal", "j"):
            try:
                tgt = int(ins.op_str, 16)
                if tgt in by_addr:
                    comment = f"  # {by_addr[tgt]}"
            except ValueError:
                pass
        print(f"  {ins.address:#010x}: {ins.bytes.hex()}  {ins.mnemonic:8s} {ins.op_str}{comment}")


if __name__ == "__main__":
    main()
