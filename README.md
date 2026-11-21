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
- `tools/` — Cross-platform build driver (`build.py`), compiler driver (`cc.py`), per-function matcher (`check.py`), disassembler (`disasm.py`), and progress tracker (`progress.py`).
- `notes/` — ROM layout map (`tu_map.md` / `tu_map.json`), engine struct notes (`engine_structs.md`), and prototype comparison notes (`proto_clues.md`).
- `Dockerfile`, `requirements.txt`, `Makefile`, `build.sh` — Reproducible build setup.

Ignored (not tracked in git): `orig/`, `toolchains/`, `asm/`, `assets/`, `build/`.

## Prerequisites

1. **Retail ROMs** (big-endian `.z64` format):
   - Place USA (`NWPE`) at `orig/us/baserom.z64`
   - Place Europe (`NWPP`) at `orig/eu/baserom.z64`
2. **Python 3.9+ dependencies**:
   ```sh
   pip install -r requirements.txt
   ```
3. **SN64 toolchain & N64 headers** (placed under `toolchains/`, or override via `WIPEOUT64_TOOLCHAIN`):
   - `toolchains/sn272_0006/` — SN Systems Psy-Q N64 compiler (`cppn64.exe`, `cc1n64.exe` `SN32.3.7.0006` / `cygnus-2.7.2-970404`, `asn64.exe` 2.72)
   - `toolchains/libreultra/include/2.0I/` — N64 SDK / `libultra` headers (`ultra64.h`, `PR/gbi.h`, etc.)
   - `toolchains/bin/psyq-obj-parser` (or `.exe` on native Windows) — converts SN LNK objects to big-endian MIPS ELF
4. **Platform-specific setup**:
   - **Linux (x86_64 / arm64)**:
     - Install `wibo` or `wine` (or place `wibo` in `toolchains/bin/wibo`, or set `WIPEOUT64_WIBO`).
     - **Native (no Docker required)**: install `binutils-mips-linux-gnu` (`sudo apt install binutils-mips-linux-gnu`) and a host-compatible `psyq-obj-parser`; `tools/cc.py` and `tools/build.py` auto-detect native binutils and skip Docker automatically.
     - **Docker fallback**: `docker build -t wipeout64-tools .` (use `--platform=linux/amd64` on arm64 if `psyq-obj-parser` is x86_64).
   - **macOS (Apple Silicon & Intel x64)**:
     - Place `wibo-macos` in `toolchains/bin/wibo-macos` (runs natively on Intel x64 and via Rosetta on Apple Silicon) or install `wine`.
     - Build the MIPS binutils container: `docker build -t wipeout64-tools .` (or `--platform=linux/amd64` if `psyq-obj-parser` is Linux x86_64 without Rosetta-for-Linux).
   - **Windows (x64)**:
     - `cppn64.exe`, `cc1n64.exe`, and `asn64.exe` execute **natively** (no `wibo` or `wine` needed).
     - Either run Docker Desktop (`docker build -t wipeout64-tools .` with Linux `toolchains/bin/psyq-obj-parser`), **or** install native `mips-linux-gnu` binutils + `toolchains/bin/psyq-obj-parser.exe` (MSYS2/WSL) with `WIPEOUT64_USE_DOCKER=0`.
     - Run builds directly with Python (no `bash` or `make` required): `python tools/build.py all`.

## Building

```sh
python3 tools/build.py all    # or: make / ./build.sh all (Windows: python tools/build.py all)
python3 tools/build.py us     # USA only
python3 tools/build.py eu     # Europe only
python3 tools/progress.py     # report matched function and byte counts
make clean                    # remove generated asm/, assets/, and build/
```

A successful build prints:

```text
OK us: 54e4795aba4fc326f79d0703ffdb51a212dd4b5c (byte-identical)
OK eu: 3eef8ec7d0009ee67d604aaca2bb61b7397ba862 (byte-identical)
```

### Environment Overrides

- `WIPEOUT64_TOOLCHAIN` — custom path to `toolchains/`
- `WIPEOUT64_WIBO` — explicit Win32 runner path (`wibo-macos`, `wibo`, `wine`; empty for native Windows)
- `WIPEOUT64_POP` — explicit path to native `psyq-obj-parser`
- `WIPEOUT64_BINUTILS_PREFIX` — host MIPS binutils prefix (default: `mips-linux-gnu-`)
- `WIPEOUT64_USE_DOCKER` — `auto` (default), `1` (force Docker), or `0` (force native host binutils)
- `WIPEOUT64_IMAGE` — Docker image name (default: `wipeout64-tools:latest`, falls back to `wipeoutxl:latest`)

## Developer Tools

```sh
python3 tools/check.py -v src/skeleton.c                 # verify every function in a TU (us + eu)
python3 tools/check.py -v -V us --only NewSkeleton src/skeleton.c
python3 tools/disasm.py NewSkeleton --version us         # annotated disassembly
python3 tools/progress.py                                # progress summary
```

Use `#ifdef VERSION_EU` / `#ifdef VERSION_US` only where the retail USA and Europe binaries genuinely differ (50 Hz PAL timing/scaling, 320×288 screen coordinates, 24 vs 20 audio channels, and `F3DLX` triangle particles after Europe dropped `L3DEX`).
