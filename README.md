# Wipeout 64 Decompilation

A matching C decompilation of **Wipeout 64** (Nintendo 64, 1998, Psygnosis / Midway). A single C codebase compiles byte-identically to both retail releases:

| Region | Game Code | Target Path | Size | SHA-1 | Game Code Matched |
|---|---|---|---|---|---|
| **USA (`us`)** | `NWPE` | `orig/us/baserom.z64` | 8,388,608 B | `54e4795aba4fc326f79d0703ffdb51a212dd4b5c` | **618 / 618 funcs** (`431,468 B`, **100.00%**) |
| **Europe (`eu`)** | `NWPP` | `orig/eu/baserom.z64` | 8,388,608 B | `3eef8ec7d0009ee67d604aaca2bb61b7397ba862` | **617 / 617 funcs** (`434,984 B`, **100.00%**) |

All **51 original game translation units** (`53` C files in `src/`) and all game `.rodata` are compiled from C with zero `INCLUDE_ASM` stubs. Static vendor libraries (`libultra` 2.0H, `libnaudio`, Software Creations `libmus`, `libgcc`), Nintendo RSP microcode (`n_aspMain`, `F3DLX`, `L3DEX`, `S2DEX`), initialized global data (`main_data`), and ROM assets (`7.12 MB`) are extracted from your retail ROMs by `splat` at build time.

This repository **does not** contain any retail ROMs, game assets, audio, textures, microcode, or proprietary compilers. You must supply your own legal copies of the retail ROM(s) and toolchain files.

## Repository Layout

- `src/` — Decompiled C source for all 51 original game translation units (53 `.c` files).
- `include/` — Game types, struct layouts, and module headers.
- `config/` — `splat` split configs (`wipeout64.yaml`), symbol tables (`symbol_addrs.txt`), and linker scripts (`syms.ld`, `common_syms.ld`) for both `us` and `eu`.
- `tools/` — Compiler driver (`cc.py`), per-function matcher (`check.py`), disassembler (`disasm.py`), and progress tracker (`progress.py`).
- `notes/` — ROM layout map (`tu_map.md` / `tu_map.json`), engine struct notes (`engine_structs.md`), and prototype comparison notes (`proto_clues.md`).
- `Dockerfile`, `requirements.txt`, `Makefile`, `build.sh` — Reproducible build setup.

Ignored (not tracked in git): `orig/`, `toolchains/`, `asm/`, `assets/`, `build/`.

## Prerequisites

1. **Retail ROMs** (big-endian `.z64` format):
   - Place USA (`NWPE`) at `orig/us/baserom.z64`
   - Place Europe (`NWPP`) at `orig/eu/baserom.z64`
2. **Python dependencies**:
   ```sh
   pip install -r requirements.txt
   ```
3. **MIPS binutils container**:
   ```sh
   docker build -t wipeout64-tools .
   ```
4. **Toolchain** (placed under `toolchains/`, or override via `WIPEOUT64_TOOLCHAIN`):
   - `toolchains/sn272_0006/` — SN Systems Psy-Q N64 compiler (`cppn64.exe`, `cc1n64.exe` `SN32.3.7.0006` / `cygnus-2.7.2-970404`, `asn64.exe` 2.72)
   - `toolchains/bin/psyq-obj-parser` — Linux binary (executed inside Docker) to convert SN LNK objects to big-endian MIPS ELF
   - `toolchains/bin/wibo-macos` or `wibo` / `wine` (override via `WIPEOUT64_WIBO`) to execute the Win32 SN64 binaries
   - `toolchains/libreultra/include/2.0I/` — N64 SDK / `libultra` headers (`ultra64.h`, `PR/gbi.h`, etc.)

## Building

```sh
make            # split (if needed), compile, link, and verify SHA-1 for both us and eu
make us         # USA only
make eu         # Europe only
make progress   # report matched function and byte counts
make clean      # remove generated asm/, assets/, and build/
```

A successful build prints:

```text
OK us: 54e4795aba4fc326f79d0703ffdb51a212dd4b5c (byte-identical)
OK eu: 3eef8ec7d0009ee67d604aaca2bb61b7397ba862 (byte-identical)
```

## Developer Tools

```sh
python3 tools/check.py -v src/skeleton.c                 # verify every function in a TU (us + eu)
python3 tools/check.py -v -V us --only NewSkeleton src/skeleton.c
python3 tools/disasm.py NewSkeleton --version us         # annotated disassembly
python3 tools/progress.py                                # progress summary
```

Use `#ifdef VERSION_EU` / `#ifdef VERSION_US` only where the retail USA and Europe binaries genuinely differ (50 Hz PAL timing/scaling, 320×288 screen coordinates, 24 vs 20 audio channels, and `F3DLX` triangle particles after Europe dropped `L3DEX`).
