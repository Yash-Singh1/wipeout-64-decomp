#!/usr/bin/env python3
"""Compile a Wipeout 64 C file and compare each function byte-for-byte against retail ROM(s).

Resolves ELF relocations (R_MIPS_26, R_MIPS_HI16, R_MIPS_LO16, R_MIPS_32) using
config/<ver>/symbol_addrs.txt + build/<ver>/wipeout64.map, then compares every
compiled C function word-for-word against orig/<ver>/baserom.z64.
"""
import argparse
import re
import struct
import sys
import tempfile
from pathlib import Path
from capstone import Cs, CS_ARCH_MIPS, CS_MODE_MIPS64, CS_MODE_BIG_ENDIAN

sys.path.insert(0, str(Path(__file__).resolve().parent))
import cc

ROOT = Path(__file__).resolve().parents[1]
MAIN_VRAM_BASE = 0x80000400
MAIN_ROM_BASE = 0x1000


def parse_elf_be32(blob):
    """Minimal big-endian 32-bit MIPS ELF parser."""
    if blob[:4] != b"\x7fELF" or blob[4] != 1 or blob[5] != 2:
        raise ValueError("Expected big-endian 32-bit ELF")
    e_shoff, _, _, _, _, e_shentsize, e_shnum, e_shstrndx = struct.unpack_from(">IIHHHHHH", blob, 32)
    sections = []
    for i in range(e_shnum):
        off = e_shoff + i * e_shentsize
        sh = struct.unpack_from(">IIIIIIIIII", blob, off)
        sections.append(sh)
    shstr_off = sections[e_shstrndx][4]
    shstr_sz = sections[e_shstrndx][5]
    shstr = blob[shstr_off:shstr_off + shstr_sz]

    def get_cstr(table, idx):
        end = table.find(b"\x00", idx)
        return table[idx:end].decode("ascii", "replace")

    names = [get_cstr(shstr, s[0]) for s in sections]
    sym_idx = next(i for i, s in enumerate(sections) if s[1] == 2)  # SHT_SYMTAB
    str_idx = sections[sym_idx][6]
    strtab = blob[sections[str_idx][4]:sections[str_idx][4] + sections[str_idx][5]]
    sym_data = blob[sections[sym_idx][4]:sections[sym_idx][4] + sections[sym_idx][5]]
    symbols = []
    for off in range(0, len(sym_data), 16):
        st_name, st_value, st_size, st_info, st_other, st_shndx = struct.unpack_from(">IIIBBH", sym_data, off)
        symbols.append((get_cstr(strtab, st_name), st_value, st_size, st_info, st_other, st_shndx))

    return sections, names, symbols


def load_version_symbols(ver):
    addrs = {}
    func_sizes = {}
    for p in [
        ROOT / "config" / "common_syms.ld",
        ROOT / "config" / ver / "symbol_addrs.txt",
        ROOT / "build" / ver / "undefined_funcs_auto.txt",
        ROOT / "build" / ver / "undefined_syms_auto.txt",
    ]:
        if not p.is_file():
            continue
        for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = re.match(r"^\s*([A-Za-z_.$][A-Za-z0-9_.$]*)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;", ln)
            if m:
                addrs[m.group(1)] = int(m.group(2), 0)

    # Pull all splat-discovered function addresses and exact sizes from asm/<ver>
    asm_dir = ROOT / "asm" / ver
    if asm_dir.is_dir():
        for p in asm_dir.rglob("*.s"):
            cur = None
            for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
                m = re.match(r"^glabel\s+(\S+)", ln)
                if m:
                    cur = [m.group(1), None, None]
                elif cur is not None:
                    m2 = re.match(r"^\s*/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s*\*/", ln)
                    if m2:
                        v = int(m2.group(1), 16)
                        if cur[1] is None:
                            cur[1] = v
                        cur[2] = v + 4
                    elif ln.startswith("endlabel "):
                        if cur[1] is not None:
                            addrs.setdefault(cur[0], cur[1])
                            func_sizes[cur[0]] = cur[2] - cur[1]
                        cur = None

    # Pull section/symbol addresses from map file
    map_p = ROOT / "build" / ver / "wipeout64.map"
    if map_p.is_file():
        for ln in map_p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = re.match(r"^\s+0x00000000([0-9a-fA-F]{8})\s+([A-Za-z_.$][A-Za-z0-9_.$]*)\s*$", ln)
            if m:
                addrs.setdefault(m.group(2), int(m.group(1), 16))
    return addrs, func_sizes


def resolve_and_compare(obj_bytes, ver, only=None, verbose=False):
    sections, names, symbols = parse_elf_be32(obj_bytes)
    addrs, func_sizes = load_version_symbols(ver)
    rom = (ROOT / "orig" / ver / "baserom.z64").read_bytes()

    text_idx = next((i for i, n in enumerate(names) if n == ".text"), None)
    if text_idx is None:
        raise ValueError("Object has no .text section")
    text_sh = sections[text_idx]
    text_buf = bytearray(obj_bytes[text_sh[4]:text_sh[4] + text_sh[5]])

    # Collect functions in .text sorted by offset
    text_funcs = []
    for st_name, st_val, st_size, st_info, st_other, st_shndx in symbols:
        if st_shndx == text_idx and st_name and not st_name.startswith(".") and not st_name.startswith("$"):
            if st_name in ("gcc2_compiled.", "__gnu_compiled_c"):
                continue
            text_funcs.append((st_val, st_name, st_size))
    text_funcs.sort()

    # Compute lengths from next symbol or section end
    func_ranges = {}
    for idx, (start, name, sz) in enumerate(text_funcs):
        end = text_funcs[idx + 1][0] if idx + 1 < len(text_funcs) else len(text_buf)
        func_ranges[name] = (start, end - start)

    # Locate .rodata base in retail ROM by matching literal bytes if present
    rodata_idx = next((i for i, n in enumerate(names) if n in (".rodata", ".rdata")), None)
    rodata_vram_base = None
    if rodata_idx is not None:
        rsh = sections[rodata_idx]
        rbytes = obj_bytes[rsh[4]:rsh[4] + rsh[5]]
        if rbytes:
            # Search inside retail .rodata region (0x10f4..0x5700)
            pos = rom.find(rbytes, 0x10F4, 0x5700)
            if pos != -1:
                rodata_vram_base = MAIN_VRAM_BASE + pos - MAIN_ROM_BASE
            else:
                # Try matching individual non-empty pieces later per relocation or first 8 bytes
                if len(rbytes) >= 4:
                    pos = rom.find(rbytes[:min(len(rbytes), 16)], 0x10F4, 0x5700)
                    if pos != -1:
                        rodata_vram_base = MAIN_VRAM_BASE + pos - MAIN_ROM_BASE

    # Parse .rel.text relocations
    rel_idx = next((i for i, s in enumerate(sections) if s[1] == 9 and s[7] == text_idx), None)
    rels = []
    if rel_idx is not None:
        rsh = sections[rel_idx]
        rdata = obj_bytes[rsh[4]:rsh[4] + rsh[5]]
        for off in range(0, len(rdata), 8):
            r_off, r_info = struct.unpack_from(">II", rdata, off)
            rels.append((r_off, r_info & 0xFF, r_info >> 8))

    def lookup_sym(sym_i, addend, func_base_vram, func_start_off):
        st_name, st_val, st_size, st_info, st_other, st_shndx = symbols[sym_i]
        if st_name in addrs:
            return addrs[st_name] + addend
        m = re.fullmatch(r"(?:D_|func_|jtbl_|L)([0-9A-Fa-f]{8})", st_name)
        if m:
            return int(m.group(1), 16) + addend
        if st_shndx == text_idx:
            # Local jump inside .text
            target_off = st_val + addend
            for fn_start, fn_name, _ in text_funcs:
                fn_s, fn_len = func_ranges[fn_name]
                if fn_s <= target_off < fn_s + fn_len and fn_name in addrs:
                    return addrs[fn_name] + (target_off - fn_s)
            return func_base_vram + (target_off - func_start_off)
        if rodata_idx is not None and st_shndx == rodata_idx and rodata_vram_base is not None:
            return rodata_vram_base + st_val + addend
        return None

    md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS64 | CS_MODE_BIG_ENDIAN)
    ok_all = True
    checked = 0

    for start, name, _ in text_funcs:
        if only and name != only:
            continue
        if name not in addrs:
            print(f"[{ver}] SKIP  {name}: not in symbol_addrs/map")
            continue
        checked += 1
        vram = addrs[name]
        rom_off = vram - MAIN_VRAM_BASE + MAIN_ROM_BASE
        c_start, c_len = func_ranges[name]
        # Trim trailing alignment nops if retail function is shorter by alignment padding
        r_len = func_sizes.get(name, c_len)
        while c_len > r_len and c_len >= 4 and text_buf[c_start + c_len - 4:c_start + c_len] == b"\x00\x00\x00\x00":
            c_len -= 4

        patched = bytearray(text_buf[c_start:c_start + c_len])
        # Apply relocations falling within [c_start, c_start + c_len)
        pending_hi = []
        unresolved = []
        for r_off, r_type, r_sym in rels:
            if not (c_start <= r_off < c_start + c_len):
                continue
            local_off = r_off - c_start
            word, = struct.unpack_from(">I", patched, local_off)
            if r_type == 4:  # R_MIPS_26
                addend = (word & 0x03FFFFFF) << 2
                val = lookup_sym(r_sym, addend, vram, c_start)
                if val is None:
                    unresolved.append((local_off, symbols[r_sym][0]))
                else:
                    word = (word & 0xFC000000) | ((val >> 2) & 0x03FFFFFF)
                    struct.pack_into(">I", patched, local_off, word)
            elif r_type == 5:  # R_MIPS_HI16
                pending_hi.append((local_off, r_sym, word & 0xFFFF))
            elif r_type == 6:  # R_MIPS_LO16
                lo_s = (word & 0xFFFF)
                if lo_s >= 0x8000:
                    lo_s -= 0x10000
                # Pair with matching pending HI16(s)
                matched_hi = [h for h in pending_hi if h[1] == r_sym]
                if matched_hi:
                    for h_off, h_sym, h_imm in matched_hi:
                        pending_hi.remove((h_off, h_sym, h_imm))
                        addend = (h_imm << 16) + lo_s
                        val = lookup_sym(r_sym, addend, vram, c_start)
                        if val is None:
                            # If rodata couldn't be matched wholesale, check literal bytes at retail target!
                            if rodata_idx is not None and symbols[r_sym][5] == rodata_idx:
                                # Compare literal content directly against retail ROM's target address!
                                rw_hi, = struct.unpack_from(">I", rom, rom_off + h_off)
                                rw_lo, = struct.unpack_from(">I", rom, rom_off + local_off)
                                r_lo_s = (rw_lo & 0xFFFF) - (0x10000 if (rw_lo & 0xFFFF) >= 0x8000 else 0)
                                r_tgt = (((rw_hi & 0xFFFF) << 16) + r_lo_s) & 0xFFFFFFFF
                                rsh = sections[rodata_idx]
                                rbytes = obj_bytes[rsh[4]:rsh[4] + rsh[5]]
                                r_rom_tgt = r_tgt - MAIN_VRAM_BASE + MAIN_ROM_BASE
                                ro_off = symbols[r_sym][1] + addend
                                # Verify at least 4 bytes match (or null-terminated string)
                                check_len = min(4, len(rbytes) - ro_off)
                                if check_len > 0 and rom[r_rom_tgt:r_rom_tgt + check_len] == rbytes[ro_off:ro_off + check_len]:
                                    val = r_tgt
                        if val is None:
                            unresolved.append((local_off, symbols[r_sym][0]))
                        else:
                            hi_out = ((val + 0x8000) >> 16) & 0xFFFF
                            lo_out = val & 0xFFFF
                            hw, = struct.unpack_from(">I", patched, h_off)
                            struct.pack_into(">I", patched, h_off, (hw & 0xFFFF0000) | hi_out)
                            struct.pack_into(">I", patched, local_off, (word & 0xFFFF0000) | lo_out)
                else:
                    val = lookup_sym(r_sym, lo_s, vram, c_start)
                    if val is None:
                        unresolved.append((local_off, symbols[r_sym][0]))
                    else:
                        struct.pack_into(">I", patched, local_off, (word & 0xFFFF0000) | (val & 0xFFFF))

        # Flush any trailing orphan R_MIPS_HI16 relocations (e.g. delay-slot copy of a loop-header lui)
        for h_off, h_sym, h_imm in pending_hi:
            # Find matching LO16 addend from an earlier relocation against the same symbol if any
            addend = 0
            for r_off2, r_type2, r_sym2 in rels:
                if r_type2 == 6 and r_sym2 == h_sym and c_start <= r_off2 < c_start + c_len:
                    w2, = struct.unpack_from(">I", text_buf, r_off2)
                    lo2 = w2 & 0xFFFF
                    if lo2 >= 0x8000:
                        lo2 -= 0x10000
                    addend = (h_imm << 16) + lo2
                    break
            val = lookup_sym(h_sym, addend, vram, c_start)
            if val is None:
                unresolved.append((h_off, symbols[h_sym][0]))
            else:
                hi_out = ((val + 0x8000) >> 16) & 0xFFFF
                hw, = struct.unpack_from(">I", patched, h_off)
                struct.pack_into(">I", patched, h_off, (hw & 0xFFFF0000) | hi_out)

        retail_bytes = rom[rom_off:rom_off + r_len]
        if not unresolved and bytes(patched) == retail_bytes:
            print(f"[{ver}] MATCH {name} ({vram:#010x}, {r_len} bytes)")
        else:
            ok_all = False
            nwords = max(len(patched), len(retail_bytes)) // 4
            diffs = 0
            for wi in range(nwords):
                b1 = retail_bytes[wi * 4:(wi + 1) * 4]
                b2 = patched[wi * 4:(wi + 1) * 4]
                if b1 != b2:
                    diffs += 1
            extra = f", unresolved={unresolved}" if unresolved else ""
            print(f"[{ver}] DIFF  {name} ({vram:#010x}, retail={r_len}B, compiled={len(patched)}B, {diffs} word diffs{extra})")
            if verbose:
                for wi in range(nwords):
                    b1 = retail_bytes[wi * 4:(wi + 1) * 4]
                    b2 = patched[wi * 4:(wi + 1) * 4]
                    mark = " " if b1 == b2 else "!"
                    i1 = list(md.disasm(b1, vram + wi * 4)) if len(b1) == 4 else []
                    i2 = list(md.disasm(b2, vram + wi * 4)) if len(b2) == 4 else []
                    s1 = f"{b1.hex():8s} {i1[0].mnemonic:7s} {i1[0].op_str}" if i1 else f"{b1.hex():8s}"
                    s2 = f"{b2.hex():8s} {i2[0].mnemonic:7s} {i2[0].op_str}" if i2 else f"{b2.hex():8s}"
                    print(f"  {mark} +{wi*4:04x}: {s1:36s} | {s2}")

    if checked == 0:
        print(f"[{ver}] No matching functions found to check.")
        return False
    return ok_all


def main():
    ap = argparse.ArgumentParser(description="Compile C source and verify byte-exact match against retail ROMs")
    ap.add_argument("source", type=Path, help="Path to C source file (e.g. src/skeleton.c)")
    ap.add_argument("--version", "-V", choices=["us", "eu", "both"], default="both")
    ap.add_argument("--only", help="Check only this function name")
    ap.add_argument("-v", "--verbose", action="store_true", help="Print side-by-side disassembly diff")
    ap.add_argument("--opt", default="-O2")
    args = ap.parse_args()

    versions = ["us", "eu"] if args.version == "both" else [args.version]
    all_ok = True
    src_text = args.source.read_text(encoding="utf-8")
    # Strip INCLUDE_ASM lines for isolated C checking
    c_only = "\n".join(
        "" if cc.INCLUDE_ASM_RE.match(ln) else ln
        for ln in src_text.splitlines()
    ) + "\n"

    for ver in versions:
        obj_bytes = cc.compile_c_to_obj_bytes(
            c_only, args.source.resolve().parent, ver, args.opt, [], []
        )
        ok = resolve_and_compare(obj_bytes, ver, only=args.only, verbose=args.verbose)
        all_ok = all_ok and ok

    sys.exit(0 if all_ok else 1)


if __name__ == "__main__":
    main()
