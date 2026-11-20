# Wipeout 64 decompilation

A matching decompilation of Wipeout 64 (Nintendo 64, 1998, Psygnosis / Midway). A single C codebase compiles byte-identically to both retail releases:

- **USA (`us`, `NWPE`)**: `orig/us/baserom.z64`, 8,388,608 bytes, SHA-1 `54e4795aba4fc326f79d0703ffdb51a212dd4b5c`
- **Europe (`eu`, `NWPP`)**: `orig/eu/baserom.z64`, 8,388,608 bytes, SHA-1 `3eef8ec7d0009ee67d604aaca2bb61b7397ba862`

Retail ROMs, build artifacts, and proprietary compilers live in `orig/`, `build/`, `asm/`, `assets/`, and `toolchains/`, and are excluded from git.

## Toolchain

Both USA and Europe game code were compiled with the SN Systems Psy-Q N64 compiler (`SN32.3.7.0006`, GNU C `cygnus-2.7.2-970404`, `-O2 -G0 -mips3 -mcpu=4300 -mgas -meb`) and assembled with SN ASN64 2.72. `libultra` 2.0H and `libnaudio` were linked from Nintendo's IDO-compiled static libraries and stay in assembly.

`tools/cc.py` runs `cppn64.exe`, `cc1n64.exe`, and `asn64.exe` natively on Apple Silicon via `wibo-macos` (Rosetta), then converts the SN LNK object to big-endian MIPS ELF via `psyq-obj-parser -b` and renames `.rdata` to `.rodata` inside the `wipeoutxl:latest` Docker container (which also provides `mips-linux-gnu-` binutils).

## Building

Place your retail ROMs at `orig/us/baserom.z64` and `orig/eu/baserom.z64`, then run:

```sh
./build.sh all   # split (if needed), compile, link, verify SHA-1 for both us and eu
./build.sh us    # USA only
./build.sh eu    # Europe only
```

`./build.sh` prints `OK us: ... (byte-identical)` and `OK eu: ... (byte-identical)` when the built ROMs equal the retail bytes.

## Working on a function

```sh
python3 tools/disasm.py NewSkeleton --version us
python3 tools/disasm.py 0x80067920 --version eu
python3 tools/check.py -v src/skeleton.c            # check both us and eu
python3 tools/check.py -v -V us --only NewSkeleton src/skeleton.c
python3 tools/progress.py                           # game code progress for us and eu
```

Use `#ifdef VERSION_EU` / `#ifdef VERSION_US` only where the retail USA and Europe binaries genuinely differ. See `AGENTS.md` for project conventions and architecture notes.
