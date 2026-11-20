# Working on the Wipeout 64 decompilation

Goal: clean, readable C source that compiles with SN64 (`cygnus-2.7.2-970404 SN32.3.7.0006` + `asn64` 2.72) to byte-identical copies of both retail ROMs from a single codebase:

- `orig/us/baserom.z64` (USA `NWPE`, SHA-1 `54e4795aba4fc326f79d0703ffdb51a212dd4b5c`)
- `orig/eu/baserom.z64` (Europe `NWPP`, SHA-1 `3eef8ec7d0009ee67d604aaca2bb61b7397ba862`)

An optional reference prototype (`orig/proto/Wipeout 64 (Europe) (Proto).n64`) may be inspected for extra debug/assert strings, but only retail `us` and `eu` are build targets.

## Ground rules

- Both `./build.sh us` and `./build.sh eu` (i.e. `./build.sh all`) must pass and print `OK ... (byte-identical)` before every commit. Never regress a matching function in either version.
- Use `#ifdef VERSION_US` / `#ifdef VERSION_EU` only for real behavioral or layout differences between retail USA and Europe. Keep shared code clean and single-sourced.
- Write readable C with real names and struct fields. Wipeout 64 is built on the Wipeout XL / 2097 engine; sibling Wipeout decomps in `../` can be used as reference for original engine struct layouts, algorithms, and symbol names.
- Never commit `orig/`, `toolchains/`, `asm/`, `assets/`, or `build/`.
- `libultra` 2.0H / `libnaudio` functions and RSP microcodes (`rspboot`, `n_aspMain`, `gspF3DLX_fifo`, `gspL3DEX_fifo`, `gspS2DEX_fifo`) stay in assembly/binary form; focus C decompilation on game translation units.

## ROM layout

Both retail ROMs use the same SN Systems `psylink` layout (`crt0` -> `.rodata` -> `.text` -> `.data` -> `.bss`):

- `0x000000..0x000040`: N64 ROM header (`NWPE` / `NWPP`, entry `0x80000400`)
- `0x000040..0x001000`: IPL3 bootcode (CIC-NUS-6102)
- `0x001000..0x0010F4`: `crt0` (`__start`, `__init_ctors`, `__fini_dtors`, VRAM `0x80000400`)
- `0x0010F4..0x005660` (US) / `0x0010F4..0x005640` (EU): `.rodata` (12 bytes `crt0_rodata` at `0x10F4`, then 16-byte aligned `main_rodata` at `0x1100`)
- `0x005660..0x088910` (US) / `0x005640..0x0886B0` (EU): `.text` (RSP microcode text -> game code -> `libultra` 2.0H)
  - USA includes `gspL3DEX_fifo`; Europe omits it.
  - Europe links `Trail.c` and one early block before `__assert`/`audio.c`; USA links `Trail.c` near the end of game code.
  - `psylink` sometimes fills 4-12 byte alignment gaps between `.o` files with uninitialized host bytes (`0xCDCDCDCD` etc.); preserve these as tiny `textbin` subsegments in `wipeout64.yaml`.
- `0x088910..0x0992F0` (US) / `0x0886B0..0x098890` (EU): `.data` (RSP microcode data -> game `.data` -> `libultra` `.data`)
- `.bss`: `0x800986F0..0x800FF0E0` (US, `0x669F0` bytes), `0x80097C90..0x800F7FF0` (EU, `0x60360` bytes)
- `0x099300..0x099550` (US) / `0x0988C0..0x098B10` (EU): `static_gfx` (NTSC vs PAL viewport/scissor display lists)
- `0x099550..0x7B6E20` (US) / `0x098B10..0x7B63E0` (EU): `assets` (byte-identical between USA and Europe)
- `0x7B6E20..0x800000` (US) / `0x7B63E0..0x800000` (EU): `0xFF` cartridge padding

## Tools

- `./build.sh [us|eu|all]`: split (when YAML/symbols change), compile C via `tools/cc.py`, assemble, link, verify SHA-1.
- `python3 tools/cc.py --version us|eu src/foo.c build/us/src/foo.c.o`: compile a C file (supports `INCLUDE_ASM("nonmatchings/<tu>", <func>)`).
- `python3 tools/check.py [-v] [-V us|eu|both] [--only FUNC] src/foo.c`: compile and verify function-by-function byte matches against retail ROMs.
- `python3 tools/disasm.py NAME|ADDR [-V us|eu|proto]`: annotated disassembly.
- `python3 tools/progress.py [--verify]`: report matched function and byte counts for USA and Europe.

## Working with partial C files

1. Split a game translation unit in both `config/us/wipeout64.yaml` and `config/eu/wipeout64.yaml` as `type: c` (or `type: asm` until ready to link C).
2. While a C file is only partially matched, keep unmatched functions as `INCLUDE_ASM("nonmatchings/<tu>", <func_name>);` in exact function order.
3. For `.rodata` strings/floats: because `psylink` grouped all `.rodata` into `0x10F4..0x5660`, when a C file emits `.rodata` literals, split its `.rodata` slice out of `main_rodata` in `wipeout64.yaml` (replace that `rodata` range with the `.c` object's `.rodata` — `splat` places `<tu>.c.o(.rodata)` in `.rodata` automatically when the subsegment is `c`!).
4. Run `python3 tools/check.py -v src/<tu>.c` to verify both `us` and `eu`, then `./build.sh all` to verify full ROM byte identity.
