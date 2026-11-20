# Wipeout 64 — Complete Translation Unit (TU) Map (USA & Europe)

Complete `.o` boundary map of both retail Nintendo 64 ROMs (`orig/us/baserom.z64` `NWPE` and `orig/eu/baserom.z64` `NWPP`), built with SN64 (`cygnus-2.7.2-970404 SN32.3.7.0006` + `asn64` 2.72 + `psylink`) plus Nintendo `libultra` 2.0H / `libnaudio` and Software Creations `libmus`.

Machine-readable version: [`notes/tu_map.json`](tu_map.json).

## ROM Layout Summary

Address conversion for all `.rodata`, `.text`, and `.data` in `main`: `VRAM = ROM + 0x7FFFF400`.

| Section | USA (`NWPE`) ROM | USA VRAM | Europe (`NWPP`) ROM | Europe VRAM |
|---|---|---|---|---|
| `crt0` `.text` | `0x01000..0x010F4` | `0x80000400..0x800004F4` | `0x01000..0x010F4` | `0x80000400..0x800004F4` |
| `.rodata` (total) | `0x010F4..0x05660` | `0x800004F4..0x80004A60` | `0x010F4..0x05640` | `0x800004F4..0x80004A40` |
| RSP microcode `.text` | `0x05660..0x09840` | `0x80004A60..0x80008C40` | `0x05640..0x08830` | `0x80004A40..0x80007C30` |
| Game `.text` | `0x09840..0x72EF0` (51 TUs, 618 fns) | `0x80008C40..0x800722F0` | `0x08830..0x72C90` (51 TUs, 617 fns) | `0x80007C30..0x80072090` |
| `libgcc` `.text` | `0x72EF0..0x73650` (3 `.o`s, 4 fns) | `0x800722F0..0x80072A50` | `0x72C90..0x733F0` (3 `.o`s, 4 fns) | `0x80072090..0x800727F0` |
| `libmus` `.text` | `0x73650..0x76CA0` (89 fns) | `0x80072A50..0x800760A0` | `0x733F0..0x76A40` (89 fns) | `0x800727F0..0x80075E40` |
| `libultra` / `libnaudio` `.text` | `0x76CA0..0x88910` (263 fns) | `0x800760A0..0x80087D10` | `0x76A40..0x886B0` (263 fns) | `0x80075E40..0x80087AB0` |
| `.data` (total) | `0x88910..0x992F0` | `0x80087D10..0x800986F0` | `0x886B0..0x98890` | `0x80087AB0..0x80097C90` |
| `.bss` | — | `0x800986F0..0x800FF0E0` | — | `0x80097C90..0x800F7FF0` |

## Key USA vs Europe Structural Differences

1. **RSP microcodes**: Europe omits `gspL3DEX_fifo` (`0x07980..0x08970` in USA).

2. **Reordered TUs (`early_game` in Europe, `0x08830..0x10670`)**: Europe links two game TUs immediately after the RSP microcodes and before `__assert`/`audio.c` (both in `.text` and in `.rodata`):

   - `effects.c` (inferred name; `'sroid'`, `'light'`): USA `0x44710..0x4A550` (`.rodata 0x2B20..0x2BC0`) → Europe `0x08830..0x0E40C` + 4B gap (`.rodata 0x1100..0x11A0`).

   - `Trail.c`: USA `0x70CF0..0x72EE4` + 12B gap (`.rodata 0x5020..0x5030`) → Europe `0x0E410..0x10670` (`.rodata 0x11A0..0x11B0`).

3. **Function count differences across all 51 game TUs (`618` USA vs `617` Europe; `711` vs `710` including `libgcc` + `libmus`)**:

   - `Track.c`: **20** (US) vs **19** (EU) — EU drops the first function (`US 0x15C20`, VRAM `0x80015020`) and its `'Convert track with track10'` string.

   - `effects.c`: **25** (US) vs **24** (EU) — EU merges/inlines `US 0x44FF4` (`0x800443F4`) into `EU 0x089E0` (`0x80007DE0`).

   - `Particle.c`: **35** (US) vs **36** (EU) — EU adds the memory-guard canary check leaf function at `EU 0x68E7C` (`0x8006827C`).

4. **Libraries inside `game_code` (`0x72EF0..0x76CA0` US / `0x72C90..0x76A40` EU)**: Immediately after the last game `.c` file (`Trail.c` in US, `sfx.c` in EU) sit 3 GCC 2.7.2 `libgcc2` objects (`_cmpdi2.o`, `_floatdidf.o`, `_udivdi3.o` + 256B `__clz_tab`) and Software Creations `libmus` (`player.o` [82 fns, 8B zero-pad] + `aud_thread`/`aud_dma`/`aud_sched` [7 fns, IDO-compiled]), right before `libultra` 2.0H (`alUnlink` at `0x76CA0` US / `0x76A40` EU).

## Boundary Proof Methodology

- `psylink` aligns every `.o`'s `.text` and `.rodata` sections to **16 bytes** (`0x10`), whereas functions inside a `.o` are 4-byte aligned. Unaligned `.o` ends are filled with 1–15 bytes of uninitialized host-heap memory (different random bytes in USA vs Europe, e.g. `0xCDCDCDCD`, `0xFAFAFAFA`, or bitmap-like byte ramps), giving **45 `.text` gaps in USA**, **47 `.text` gaps in Europe**, and **39 `.rodata` gaps in each ROM**.

- Combined across both ROMs, 47 of the 51 game TU boundaries have a non-zero `.text` padding gap in USA, Europe, or both, or a US↔EU link-order move. The remaining **4** zero-`.text`-pad boundaries (where `.text` landed on a 16-byte multiple in both ROMs) are each independently proven:

  1. `Error.c` → `pad.c` (`US 0x0FE20` / `EU 0x16C40`): 8-byte non-zero `.rodata` `psylink` gap (`US 0x1648..0x1650`, `EU 0x16F8..0x1700`) separates `'Error.c'` from the controller button strings (`'A'`, `'B'`, `'BOTTOM C'`, ...).

  2. `Tim.c` → `gfx2d.c` (`US 0x35760` / `EU 0x3CE50`): 7-byte non-zero `.rodata` gap (`US 0x2599..0x25A0`, `EU 0x2629..0x2630`) followed by `$Revision: 1.17 $` of `gfx2d.c`; the function at `US 0x35760..0x35964` ends off-16B right before `gfx2d.c`'s `'gfx2d.c'`-referencing function at `0x35964`.

  3. `plasma.c` → `frontend.c` (`US 0x524C0` / `EU 0x54250`): 4-byte non-zero `.rodata` gap (`US 0x2E9C..0x2EA0`, `EU 0x2E8C..0x2E90`) followed by `$Revision: 1.17 $` of `frontend.c`; `US 0x51B10..0x524C0` shares `plasma.c`'s `.data` (`0x800922F0..0x80092314`) and `.bss` buffers (`0x800C4630..0x800C4E68`), whereas `0x524C0` is called 12+ times exclusively from `frontend.c` and starts `frontend.c`'s `.data` (`0x80092320+`).

  4. `trackrend.c` → `n64init.c` (`US 0x667B0` / `EU 0x68520`): 8-byte non-zero `.rodata` gap (`US 0x4848..0x4850`, `EU 0x4838..0x4840`) followed by `$Revision: 1.17 $` of `n64init.c`; `US 0x667B0` (`0x80065BB0`) / `EU 0x68520` (`0x80067920`) is `main()`.

- **Zero-violation check**: Every single `.rodata` reference across all 711 USA functions and all 710 Europe functions falls strictly inside its assigned TU's `.rodata` slice (zero cross-TU violations), and 100% of `.rodata` and `.text` bytes are accounted for with zero gaps or overlaps.

## Master Translation Unit Table (USA Link Order)

Name convention: names marked `✓` come verbatim from ROM `.rodata` strings (or known standard libraries); names marked `~` are inferred descriptive names for TUs with no `.c` filename string in `.rodata`.

| US# | EU# | TU Name | Kind | Fns (US/EU) | US `.rodata` ROM (VRAM) | US `.text` ROM (VRAM) | US `.text` Pad | EU `.rodata` ROM (VRAM) | EU `.text` ROM (VRAM) | EU `.text` Pad |
|---:|---:|---|---|---:|---|---|---|---|---|---|
| 0 | 0 | `crt0.s` (✓) | boot | 3 | `0x10F4..0x1100` (`0x800004F4..0x80000500`) | `0x01000..0x010F4` (`0x80000400..0x800004F4`) | 0B | `0x10F4..0x1100` (`0x800004F4..0x80000500`) | `0x01000..0x010F4` (`0x80000400..0x800004F4`) | 0B |
| 1 | 1 | `rspboot` (✓) | rsp | 0 | — | `0x05660..0x05730` (`0x80004A60..0x80004B30`) | 0B | — | `0x05640..0x05710` (`0x80004A40..0x80004B10`) | 0B |
| 2 | 2 | `n_aspMain` (✓) | rsp | 0 | — | `0x05730..0x06550` (`0x80004B30..0x80005950`) | 0B | — | `0x05710..0x06530` (`0x80004B10..0x80005930`) | 0B |
| 3 | 3 | `gspF3DLX_fifo` (✓) | rsp | 0 | — | `0x06550..0x07980` (`0x80005950..0x80006D80`) | 0B | — | `0x06530..0x07960` (`0x80005930..0x80006D60`) | 0B |
| 4 | — | `gspL3DEX_fifo` (✓) | rsp | 0/— | — | `0x07980..0x08970` (`0x80006D80..0x80007D70`) | 0B | — | — | 0B |
| 5 | 4 | `gspS2DEX_fifo` (✓) | rsp | 0 | — | `0x08970..0x09840` (`0x80007D70..0x80008C40`) | 0B | — | `0x07960..0x08830` (`0x80006D60..0x80007C30`) | 0B |
| 6 | 7 | `assert.c` (✓) | game | 1 | — | `0x09840..0x09848` (`0x80008C40..0x80008C48`) | 8B @0x09848 (heap) | — | `0x10670..0x10678` (`0x8000FA70..0x8000FA78`) | 8B @0x10678 (heap) |
| 7 | 8 | `audio.c` (✓) | game | 10 | `0x1100..0x1140` (`0x80000500..0x80000540`) | `0x09850..0x0A0C8` (`0x80008C50..0x800094C8`) | 8B @0x0A0C8 (heap) | `0x11B0..0x11F0` (`0x800005B0..0x800005F0`) | `0x10680..0x10EF8` (`0x8000FA80..0x800102F8`) | 8B @0x10EF8 (heap) |
| 8 | 9 | `gfx.c` (✓) | game | 3 | `0x1140..0x1180` (`0x80000540..0x80000580`) | `0x0A0D0..0x0A8A4` (`0x800094D0..0x80009CA4`) | 12B @0x0A8A4 (heap) | `0x11F0..0x1230` (`0x800005F0..0x80000630`) | `0x10F00..0x116CC` (`0x80010300..0x80010ACC`) | 4B @0x116CC (heap) |
| 9 | 10 | `Scene.c` (✓) | game | 24 | `0x1180..0x1630` (`0x80000580..0x80000A30`) | `0x0A8B0..0x0F51C` (`0x80009CB0..0x8000E91C`) | 4B @0x0F51C (heap) | `0x1230..0x16E0` (`0x80000630..0x80000AE0`) | `0x116D0..0x1633C` (`0x80010AD0..0x8001573C`) | 4B @0x1633C (heap) |
| 10 | 11 | `files.c` (✓) | game | 11 | `0x1630..0x1640` (`0x80000A30..0x80000A40`) | `0x0F520..0x0FD0C` (`0x8000E920..0x8000F10C`) | 4B @0x0FD0C (heap) | `0x16E0..0x16F0` (`0x80000AE0..0x80000AF0`) | `0x16340..0x16B2C` (`0x80015740..0x80015F2C`) | 4B @0x16B2C (heap) |
| 11 | 12 | `Error.c` (✓) | game | 6 | `0x1640..0x1650` (`0x80000A40..0x80000A50`) | `0x0FD10..0x0FE20` (`0x8000F110..0x8000F220`) | 0B | `0x16F0..0x1700` (`0x80000AF0..0x80000B00`) | `0x16B30..0x16C40` (`0x80015F30..0x80016040`) | 0B |
| 12 | 13 | `pad.c` (~) | game | 3 | `0x1650..0x16A0` (`0x80000A50..0x80000AA0`) | `0x0FE20..0x103D4` (`0x8000F220..0x8000F7D4`) | 12B @0x103D4 (heap) | `0x1700..0x1750` (`0x80000B00..0x80000B50`) | `0x16C40..0x171F4` (`0x80016040..0x800165F4`) | 12B @0x171F4 (heap) |
| 13 | 14 | `Init.c` (✓) | game | 9 | `0x16A0..0x17D0` (`0x80000AA0..0x80000BD0`) | `0x103E0..0x11AE8` (`0x8000F7E0..0x80010EE8`) | 8B @0x11AE8 (heap) | `0x1750..0x1880` (`0x80000B50..0x80000C80`) | `0x17200..0x18908` (`0x80016600..0x80017D08`) | 8B @0x18908 (heap) |
| 14 | 15 | `decomp.c` (~) | game | 1 | — | `0x11AF0..0x11C94` (`0x80010EF0..0x80011094`) | 12B @0x11C94 (heap) | — | `0x18910..0x18AB4` (`0x80017D10..0x80017EB4`) | 12B @0x18AB4 (heap) |
| 15 | 16 | `Ships.c` (✓) | game | 10 | `0x17D0..0x1890` (`0x80000BD0..0x80000C90`) | `0x11CA0..0x1582C` (`0x800110A0..0x80014C2C`) | 4B @0x1582C (heap) | `0x1880..0x1940` (`0x80000C80..0x80000D40`) | `0x18AC0..0x1C7E0` (`0x80017EC0..0x8001BBE0`) | 0B |
| 16 | 17 | `math_sfx.c` (~) | game | 11 | `0x1890..0x18D0` (`0x80000C90..0x80000CD0`) | `0x15830..0x15C1C` (`0x80014C30..0x8001501C`) | 4B @0x15C1C (heap) | `0x1940..0x1980` (`0x80000D40..0x80000D80`) | `0x1C7E0..0x1CBCC` (`0x8001BBE0..0x8001BFCC`) | 4B @0x1CBCC (heap) |
| 17 | 18 | `Track.c` (✓) | game | 20/19 | `0x18D0..0x1AE0` (`0x80000CD0..0x80000EE0`) | `0x15C20..0x18FA8` (`0x80015020..0x800183A8`) | 8B @0x18FA8 (heap) | `0x1980..0x1B70` (`0x80000D80..0x80000F70`) | `0x1CBD0..0x1FEE8` (`0x8001BFD0..0x8001F2E8`) | 8B @0x1FEE8 (heap) |
| 18 | 19 | `Initrace.c` (✓) | game | 6 | `0x1AE0..0x1BA0` (`0x80000EE0..0x80000FA0`) | `0x18FB0..0x19C08` (`0x800183B0..0x80019008`) | 8B @0x19C08 (heap) | `0x1B70..0x1C30` (`0x80000F70..0x80001030`) | `0x1FEF0..0x20B48` (`0x8001F2F0..0x8001FF48`) | 8B @0x20B48 (heap) |
| 19 | 20 | `Wadfile.c` (✓) | game | 5 | `0x1BA0..0x1BB0` (`0x80000FA0..0x80000FB0`) | `0x19C10..0x19F48` (`0x80019010..0x80019348`) | 8B @0x19F48 (heap) | `0x1C30..0x1C40` (`0x80001030..0x80001040`) | `0x20B50..0x20E88` (`0x8001FF50..0x80020288`) | 8B @0x20E88 (heap) |
| 20 | 21 | `unk_19f50.c` (~) | game | 2 | `0x1BB0..0x1BF0` (`0x80000FB0..0x80000FF0`) | `0x19F50..0x1A0E4` (`0x80019350..0x800194E4`) | 12B @0x1A0E4 (heap) | `0x1C40..0x1C80` (`0x80001040..0x80001080`) | `0x20E90..0x21024` (`0x80020290..0x80020424`) | 12B @0x21024 (heap) |
| 21 | 22 | `Skeleton.c` (✓) | game | 9 | `0x1BF0..0x1C40` (`0x80000FF0..0x80001040`) | `0x1A0F0..0x1AB8C` (`0x800194F0..0x80019F8C`) | 4B @0x1AB8C (heap) | `0x1C80..0x1CD0` (`0x80001080..0x800010D0`) | `0x21030..0x21ACC` (`0x80020430..0x80020ECC`) | 4B @0x21ACC (heap) |
| 22 | 23 | `Radscn.c` (✓) | game | 3 | `0x1C40..0x1D10` (`0x80001040..0x80001110`) | `0x1AB90..0x1B804` (`0x80019F90..0x8001AC04`) | 12B @0x1B804 (heap) | `0x1CD0..0x1DA0` (`0x800010D0..0x800011A0`) | `0x21AD0..0x22744` (`0x80020ED0..0x80021B44`) | 12B @0x22744 (heap) |
| 23 | 24 | `camera.c` (~) | game | 12 | — | `0x1B810..0x1D418` (`0x8001AC10..0x8001C818`) | 8B @0x1D418 (heap) | — | `0x22750..0x24440` (`0x80021B50..0x80023840`) | 0B |
| 24 | 25 | `utils.c` (~) | game | 13 | — | `0x1D420..0x1DBCC` (`0x8001C820..0x8001CFCC`) | 4B @0x1DBCC (heap) | — | `0x24440..0x24BEC` (`0x80023840..0x80023FEC`) | 4B @0x24BEC (heap) |
| 25 | 26 | `Dynam.c` (✓) | game | 7 | `0x1D10..0x1DA0` (`0x80001110..0x800011A0`) | `0x1DBD0..0x20308` (`0x8001CFD0..0x8001F708`) | 8B @0x20308 (heap) | `0x1DA0..0x1E30` (`0x800011A0..0x80001230`) | `0x24BF0..0x275A4` (`0x80023FF0..0x800269A4`) | 12B @0x275A4 (heap) |
| 26 | 27 | `shipctrl.c` (~) | game | 6 | `0x1DA0..0x1DE0` (`0x800011A0..0x800011E0`) | `0x20310..0x21834` (`0x8001F710..0x80020C34`) | 12B @0x21834 (heap) | `0x1E30..0x1E70` (`0x80001230..0x80001270`) | `0x275B0..0x28B24` (`0x800269B0..0x80027F24`) | 12B @0x28B24 (heap) |
| 27 | 28 | `control.c` (~) | game | 12 | `0x1DE0..0x1E50` (`0x800011E0..0x80001250`) | `0x21840..0x243A4` (`0x80020C40..0x800237A4`) | 12B @0x243A4 (heap) | `0x1E70..0x1EE0` (`0x80001270..0x800012E0`) | `0x28B30..0x2B848` (`0x80027F30..0x8002AC48`) | 8B @0x2B848 (heap) |
| 28 | 29 | `collide.c` (✓) | game | 12 | `0x1E50..0x1EB0` (`0x80001250..0x800012B0`) | `0x243B0..0x27B78` (`0x800237B0..0x80026F78`) | 8B @0x27B78 (heap) | `0x1EE0..0x1F40` (`0x800012E0..0x80001340`) | `0x2B850..0x2F21C` (`0x8002AC50..0x8002E61C`) | 4B @0x2F21C (heap) |
| 29 | 30 | `shipefx.c` (~) | game | 7 | `0x1EB0..0x1EE0` (`0x800012B0..0x800012E0`) | `0x27B80..0x2871C` (`0x80026F80..0x80027B1C`) | 4B @0x2871C (heap) | `0x1F40..0x1F70` (`0x80001340..0x80001370`) | `0x2F220..0x2FDBC` (`0x8002E620..0x8002F1BC`) | 4B @0x2FDBC (heap) |
| 30 | 31 | `droid.c` (~) | game | 6 | — | `0x28720..0x29610` (`0x80027B20..0x80028A10`) | 0B | — | `0x2FDC0..0x30D14` (`0x8002F1C0..0x80030114`) | 12B @0x30D14 (heap) |
| 31 | 32 | `scnanim.c` (~) | game | 2 | `0x1EE0..0x1F00` (`0x800012E0..0x80001300`) | `0x29610..0x2A388` (`0x80028A10..0x80029788`) | 8B @0x2A388 (heap) | `0x1F70..0x1F90` (`0x80001370..0x80001390`) | `0x30D20..0x31A98` (`0x80030120..0x80030E98`) | 8B @0x31A98 (heap) |
| 32 | 33 | `Race.c` (✓) | game | 3 | `0x1F00..0x1FC0` (`0x80001300..0x800013C0`) | `0x2A390..0x2AF1C` (`0x80029790..0x8002A31C`) | 4B @0x2AF1C (heap) | `0x1F90..0x2050` (`0x80001390..0x80001450`) | `0x31AA0..0x3262C` (`0x80030EA0..0x80031A2C`) | 4B @0x3262C (heap) |
| 33 | 34 | `hud.c` (✓) | game | 19 | `0x1FC0..0x23C0` (`0x800013C0..0x800017C0`) | `0x2AF20..0x34E88` (`0x8002A320..0x80034288`) | 8B @0x34E88 (heap) | `0x2050..0x2450` (`0x80001450..0x80001850`) | `0x32630..0x3C57C` (`0x80031A30..0x8003B97C`) | 4B @0x3C57C (heap) |
| 34 | 35 | `loader.c` (✓) | game | 2 | `0x23C0..0x2500` (`0x800017C0..0x80001900`) | `0x34E90..0x355C0` (`0x80034290..0x800349C0`) | 0B | `0x2450..0x2590` (`0x80001850..0x80001990`) | `0x3C580..0x3CCA8` (`0x8003B980..0x8003C0A8`) | 8B @0x3CCA8 (heap) |
| 35 | 36 | `Tim.c` (✓) | game | 2 | `0x2500..0x25A0` (`0x80001900..0x800019A0`) | `0x355C0..0x35760` (`0x800349C0..0x80034B60`) | 0B | `0x2590..0x2630` (`0x80001990..0x80001A30`) | `0x3CCB0..0x3CE50` (`0x8003C0B0..0x8003C250`) | 0B |
| 36 | 37 | `gfx2d.c` (✓) | game | 4 | `0x25A0..0x2630` (`0x800019A0..0x80001A30`) | `0x35760..0x3624C` (`0x80034B60..0x8003564C`) | 4B @0x3624C (heap) | `0x2630..0x26C0` (`0x80001A30..0x80001AC0`) | `0x3CE50..0x3D93C` (`0x8003C250..0x8003CD3C`) | 4B @0x3D93C (heap) |
| 37 | 38 | `font.c` (~) | game | 11 | `0x2630..0x26F0` (`0x80001A30..0x80001AF0`) | `0x36250..0x374D4` (`0x80035650..0x800368D4`) | 12B @0x374D4 (heap) | `0x26C0..0x2780` (`0x80001AC0..0x80001B80`) | `0x3D940..0x3EBC4` (`0x8003CD40..0x8003DFC4`) | 12B @0x3EBC4 (heap) |
| 38 | 39 | `weapon.c` (~) | game | 59 | `0x26F0..0x2B20` (`0x80001AF0..0x80001F20`) | `0x374E0..0x44708` (`0x800368E0..0x80043B08`) | 8B @0x44708 (heap) | `0x2780..0x2BB0` (`0x80001B80..0x80001FB0`) | `0x3EBD0..0x4C42C` (`0x8003DFD0..0x8004B82C`) | 4B @0x4C42C (heap) |
| 39 | 5 ★ | `effects.c` (~) | game | 25/24 | `0x2B20..0x2BC0` (`0x80001F20..0x80001FC0`) | `0x44710..0x4A550` (`0x80043B10..0x80049950`) | 0B | `0x1100..0x11A0` (`0x80000500..0x800005A0`) | `0x08830..0x0E40C` (`0x80007C30..0x8000D80C`) | 4B @0x0E40C (heap) |
| 40 | 40 | `cont.c` (~) | game | 7 | — | `0x4A550..0x4AB5C` (`0x80049950..0x80049F5C`) | 4B @0x4AB5C (heap) | — | `0x4C430..0x4CA3C` (`0x8004B830..0x8004BE3C`) | 4B @0x4CA3C (heap) |
| 41 | 41 | `sched.c` (~) | game | 3 | — | `0x4AB60..0x4AC24` (`0x80049F60..0x8004A024`) | 12B @0x4AC24 (heap) | — | `0x4CA40..0x4CB04` (`0x8004BE40..0x8004BF04`) | 12B @0x4CB04 (heap) |
| 42 | 42 | `cpu.c` (✓) | game | 42 | `0x2BC0..0x2D40` (`0x80001FC0..0x80002140`) | `0x4AC30..0x50258` (`0x8004A030..0x8004F658`) | 8B @0x50258 (heap) | `0x2BB0..0x2D30` (`0x80001FB0..0x80002130`) | `0x4CB10..0x51FEC` (`0x8004BF10..0x800513EC`) | 4B @0x51FEC (heap) |
| 43 | 43 | `n64render.c` (✓) | game | 5 | `0x2D40..0x2D50` (`0x80002140..0x80002150`) | `0x50260..0x50738` (`0x8004F660..0x8004FB38`) | 8B @0x50738 (heap) | `0x2D30..0x2D40` (`0x80002130..0x80002140`) | `0x51FF0..0x524C8` (`0x800513F0..0x800518C8`) | 8B @0x524C8 (heap) |
| 44 | 44 | `unk_50740.c` (~) | game | 1 | `0x2D50..0x2D70` (`0x80002150..0x80002170`) | `0x50740..0x50748` (`0x8004FB40..0x8004FB48`) | 8B @0x50748 (heap) | `0x2D40..0x2D60` (`0x80002140..0x80002160`) | `0x524D0..0x524D8` (`0x800518D0..0x800518D8`) | 8B @0x524D8 (heap) |
| 45 | 45 | `plasma.c` (✓) | game | 10 | `0x2D70..0x2EA0` (`0x80002170..0x800022A0`) | `0x50750..0x524C0` (`0x8004FB50..0x800518C0`) | 0B | `0x2D60..0x2E90` (`0x80002160..0x80002290`) | `0x524E0..0x54250` (`0x800518E0..0x80053650`) | 0B |
| 46 | 46 | `frontend.c` (✓) | game | 112 | `0x2EA0..0x4800` (`0x800022A0..0x80003C00`) | `0x524C0..0x654E8` (`0x800518C0..0x800648E8`) | 8B @0x654E8 (heap) | `0x2E90..0x47F0` (`0x80002290..0x80003BF0`) | `0x54250..0x6725C` (`0x80053650..0x8006665C`) | 4B @0x6725C (heap) |
| 47 | 47 | `trackrend.c` (~) | game | 5 | `0x4800..0x4850` (`0x80003C00..0x80003C50`) | `0x654F0..0x667B0` (`0x800648F0..0x80065BB0`) | 0B | `0x47F0..0x4840` (`0x80003BF0..0x80003C40`) | `0x67260..0x68520` (`0x80066660..0x80067920`) | 0B |
| 48 | 48 | `n64init.c` (✓) | game | 7 | `0x4850..0x48D0` (`0x80003C50..0x80003CD0`) | `0x667B0..0x670A0` (`0x80065BB0..0x800664A0`) | 0B | `0x4840..0x48C0` (`0x80003C40..0x80003CC0`) | `0x68520..0x68E2C` (`0x80067920..0x8006822C`) | 4B @0x68E2C (heap) |
| 49 | 49 | `Particle.c` (✓) | game | 35/36 | `0x48D0..0x4920` (`0x80003CD0..0x80003D20`) | `0x670A0..0x6B780` (`0x800664A0..0x8006AB80`) | 0B | `0x48C0..0x4910` (`0x80003CC0..0x80003D10`) | `0x68E30..0x6D7C4` (`0x80068230..0x8006CBC4`) | 12B @0x6D7C4 (heap) |
| 50 | 50 | `unk_6b780.c` (~) | game | 2 | — | `0x6B780..0x6B90C` (`0x8006AB80..0x8006AD0C`) | 4B @0x6B90C (heap) | — | `0x6D7D0..0x6D95C` (`0x8006CBD0..0x8006CD5C`) | 4B @0x6D95C (heap) |
| 51 | 51 | `hiscores.c` (~) | game | 5 | `0x4920..0x4BB0` (`0x80003D20..0x80003FB0`) | `0x6B910..0x6CD28` (`0x8006AD10..0x8006C128`) | 8B @0x6CD28 (heap) | `0x4910..0x4BA0` (`0x80003D10..0x80003FA0`) | `0x6D960..0x6ED88` (`0x8006CD60..0x8006E188`) | 8B @0x6ED88 (heap) |
| 52 | 52 | `Racefunc.c` (✓) | game | 16 | `0x4BB0..0x4D50` (`0x80003FB0..0x80004150`) | `0x6CD30..0x6FB0C` (`0x8006C130..0x8006EF0C`) | 4B @0x6FB0C (heap) | `0x4BA0..0x4D40` (`0x80003FA0..0x80004140`) | `0x6ED90..0x71AAC` (`0x8006E190..0x80070EAC`) | 4B @0x71AAC (zero) |
| 53 | 53 | `pause.c` (~) | game | 5 | `0x4D50..0x4EB0` (`0x80004150..0x800042B0`) | `0x6FB10..0x70138` (`0x8006EF10..0x8006F538`) | 8B @0x70138 (heap) | `0x4D40..0x4EA0` (`0x80004140..0x800042A0`) | `0x71AB0..0x720D8` (`0x80070EB0..0x800714D8`) | 8B @0x720D8 (heap) |
| 54 | 54 | `inspect.c` (✓) | game | 11 | `0x4EB0..0x5000` (`0x800042B0..0x80004400`) | `0x70140..0x70848` (`0x8006F540..0x8006FC48`) | 8B @0x70848 (heap) | `0x4EA0..0x4FF0` (`0x800042A0..0x800043F0`) | `0x720E0..0x727E8` (`0x800714E0..0x80071BE8`) | 8B @0x727E8 (heap) |
| 55 | 55 | `sfx.c` (~) | game | 7 | `0x5000..0x5020` (`0x80004400..0x80004420`) | `0x70850..0x70CEC` (`0x8006FC50..0x800700EC`) | 4B @0x70CEC (heap) | `0x4FF0..0x5010` (`0x800043F0..0x80004410`) | `0x727F0..0x72C8C` (`0x80071BF0..0x8007208C`) | 4B @0x72C8C (heap) |
| 56 | 6 ★ | `Trail.c` (✓) | game | 9 | `0x5020..0x5030` (`0x80004420..0x80004430`) | `0x70CF0..0x72EE4` (`0x800700F0..0x800722E4`) | 12B @0x72EE4 (heap) | `0x11A0..0x11B0` (`0x800005A0..0x800005B0`) | `0x0E410..0x10670` (`0x8000D810..0x8000FA70`) | 0B |
| 57 | 56 | `libgcc (_cmpdi2.o)` (✓) | libgcc | 1 | — | `0x72EF0..0x72F30` (`0x800722F0..0x80072330`) | 0B | — | `0x72C90..0x72CD0` (`0x80072090..0x800720D0`) | 0B |
| 58 | 57 | `libgcc (_floatdidf.o)` (✓) | libgcc | 1 | `0x5030..0x5050` (`0x80004430..0x80004450`) | `0x72F30..0x72FEC` (`0x80072330..0x800723EC`) | 4B @0x72FEC (heap) | `0x5010..0x5030` (`0x80004410..0x80004430`) | `0x72CD0..0x72D8C` (`0x800720D0..0x8007218C`) | 4B @0x72D8C (heap) |
| 59 | 58 | `libgcc (_udivdi3.o)` (✓) | libgcc | 2 | `0x5050..0x5150` (`0x80004450..0x80004550`) | `0x72FF0..0x7364C` (`0x800723F0..0x80072A4C`) | 4B @0x7364C (heap) | `0x5030..0x5130` (`0x80004430..0x80004530`) | `0x72D90..0x733EC` (`0x80072190..0x800727EC`) | 4B @0x733EC (heap) |
| 60 | 59 | `libmus (player.o)` (✓) | libmus | 82 | `0x5150..0x5210` (`0x80004550..0x80004610`) | `0x73650..0x76208` (`0x80072A50..0x80075608`) | 8B @0x76208 (zero) | `0x5130..0x51F0` (`0x80004530..0x800045F0`) | `0x733F0..0x75FA8` (`0x800727F0..0x800753A8`) | 8B @0x75FA8 (zero) |
| 61 | 60 | `libmus (aud_thread/dma/sched)` (✓) | libmus | 7 | `0x5210..0x5220` (`0x80004610..0x80004620`) | `0x76210..0x76CA0` (`0x80075610..0x800760A0`) | 0B | `0x51F0..0x5200` (`0x800045F0..0x80004600`) | `0x75FB0..0x76A40` (`0x800753B0..0x80075E40`) | 0B |
| 62 | 61 | `libultra 2.0H / libnaudio` (✓) | libultra | 263 | `0x5220..0x5660` (`0x80004620..0x80004A60`) | `0x76CA0..0x88910` (`0x800760A0..0x80087D10`) | 0B | `0x5200..0x5640` (`0x80004600..0x80004A40`) | `0x76A40..0x886B0` (`0x80075E40..0x80087AB0`) | 0B |

*`★` = moved to `early_game` (`0x08830..0x10670`) in Europe.*

## Translation Unit Notes

- **`assert.c`** (known lib, US#6 / EU#7): __assert infinite loop stub (matched in src/assert.c)
- **`audio.c`** (ROM string, US#7 / EU#8): Audio init/streaming/heap setup ($Revision: 1.17, 7.27, 1.5; 'audio.c')
- **`gfx.c`** (ROM string, US#8 / EU#9): Graphics setup / display init ($Revision: 1.17; 'gfx.c')
- **`Scene.c`** (ROM string, US#9 / EU#10): PRM/scene loader and object renderer ($Revision: 1.17; 'Scene.c'/'scene.c')
- **`files.c`** (ROM string, US#10 / EU#11): Dynamic heap allocator (DAlloc/DFree) and file I/O ('files.c')
- **`Error.c`** (ROM string, US#11 / EU#12): Error handling ('Error.c'); 0B .text pad in US/EU, 8B .rodata psylink pad proves boundary
- **`pad.c`** (inferred name, US#12 / EU#13): Controller button mapping / input names ('A', 'B', 'BOTTOM C', 'TOP C', 'UNKNOWN INDEX', 'UNKNOWN ACTION')
- **`Init.c`** (ROM string, US#13 / EU#14): Texture/palette allocation and TIM decompression ('Init.c')
- **`decomp.c`** (inferred name, US#14 / EU#15): 1-function decompressor called exclusively by Init.c (LZ/RNC)
- **`Ships.c`** (ROM string, US#15 / EU#16): Ship initialization and physics setup ($Revision: 7.27, 1.5, 1.34; 'Ships.c')
- **`math_sfx.c`** (inferred name, US#16 / EU#17): Widely called trig/math & libmus audio helpers ($Revision: 7.27, 1.5, 1.34; 251 call sites)
- **`Track.c`** (ROM string, US#17 / EU#18): Track vertices/faces/sections/viewlist ($Revision: 1.34, 1.5, 1.17; 'Track.c'); EU drops first func (0x15C20) + 'Convert track with track10'
- **`Initrace.c`** (ROM string, US#18 / EU#19): Race/checkpoint initialization ('Initrace.c'/'initrace.c')
- **`Wadfile.c`** (ROM string, US#19 / EU#20): WAD archive loader ('Wadfile.c')
- **`unk_19f50.c`** (inferred name, US#20 / EU#21): 2-function helper called by math_sfx ($Revision: 7.27, 1.5, 1.34)
- **`Skeleton.c`** (ROM string, US#21 / EU#22): Skeleton transform hierarchy (NewSkeleton, SetSkeletonPosition/Direction; $Revision: 7.27, 1.5, 1.34; 'Skeleton.c')
- **`Radscn.c`** (ROM string, US#22 / EU#23): SceneRadiusCheck spatial object culling ('Radscn.c'/'Radius.c')
- **`camera.c`** (inferred name, US#23 / EU#24): Race camera update/modes (12 functions; called by Initrace.c and Race.c)
- **`utils.c`** (inferred name, US#24 / EU#25): Core math/vector/section/randy utilities (13 functions; 219 call sites)
- **`Dynam.c`** (ROM string, US#25 / EU#26): Ship dynamics / checkpoint logic ($Revision: 7.27, 1.5, 1.34, 1.17; 'Dynam.c')
- **`shipctrl.c`** (inferred name, US#26 / EU#27): Ship update/control dispatch ($Revision: 1.17, 7.27, 1.5; 6 functions)
- **`control.c`** (inferred name, US#27 / EU#28): Player/AI ship flight & track control (12 functions; jump table + float constants)
- **`collide.c`** (ROM string, US#28 / EU#29): Ship-track and ship-ship collision ($Revision: 1.17; 'collide.c'; 'playerShip', 'facePtr')
- **`shipefx.c`** (inferred name, US#29 / EU#30): Ship engine/damage/timer effects (7 functions; 60.0f US vs 50.0f EU)
- **`droid.c`** (inferred name, US#30 / EU#31): Rescue droid AI & skeleton movement (6 functions; 12B EU gap proves boundary)
- **`scnanim.c`** (inferred name, US#31 / EU#32): Track object/sky prefix matching ('B_', 'A_', 'D_', 'C_', 'V_', 'Z_', 'H_', 'J_'; 2 functions)
- **`Race.c`** (ROM string, US#32 / EU#33): Race setup & loop orchestration ($Revision: 1.17, 7.27, 1.5; 'Race.c')
- **`hud.c`** (ROM string, US#33 / EU#34): In-race HUD, timers, warnings, challenge/race stats ($Revision: 1.17; 'hud.c')
- **`loader.c`** (ROM string, US#34 / EU#35): Loading screen & Rumble Pak prompt ($Revision: 1.17, 7.27, 1.5; 'loader.c'; 8B EU gap proves boundary)
- **`Tim.c`** (ROM string, US#35 / EU#36): TIM texture loading ('Tim.c'); 0B .text pad in US/EU, 7B .rodata pad + gfx2d $Revision prove boundary
- **`gfx2d.c`** (ROM string, US#36 / EU#37): 2D sprite/display-list renderer ($Revision: 1.17; 'gfx2d.c')
- **`font.c`** (inferred name, US#37 / EU#38): Text/font rendering & class/track name tables ('VECTOR'..'SCREW UP', 'KLIES BRIDGE', '%i'; 225 call sites)
- **`weapon.c`** (inferred name, US#38 / EU#39): Weapons system ($Revision: 1.17, 7.27, 1.5; 59 functions, heavy switch jump tables)
- **`effects.c`** (inferred name, US#39 / EU#5): Effects/objects/explosions ('sroid', 'light'); MOVED to early_game in EU; 25 US / 24 EU fns (EU merges US 0x44FF4 into 0x089E0)
- **`cont.c`** (inferred name, US#40 / EU#40): N64 Controller/Rumble/Pak thread & SI manager (7 functions; calls osCreateThread/osMotor*)
- **`sched.c`** (inferred name, US#41 / EU#41): N64 message-queue/scheduler helper (3 functions; calls osRecvMesg/osSendMesg)
- **`cpu.c`** (ROM string, US#42 / EU#42): Main frame/CPU loop & AI/render coordination ($Revision: 1.17, 7.27, 1.5, 1.34; 'cpu.c'; 42 functions)
- **`n64render.c`** (ROM string, US#43 / EU#43): N64 RDP/RSP display-list renderer ('n64render.c'; 5 functions)
- **`unk_50740.c`** (inferred name, US#44 / EU#44): 1 uncalled 8-byte function (return a2) with $Revision: 1.17 in .rodata
- **`plasma.c`** (ROM string, US#45 / EU#45): Frontend plasma background effect ($Revision: 1.17, 7.27, 1.5; 'plasma.c'; 10 functions; .data/.rodata prove boundary at 0x524C0/0x54250)
- **`frontend.c`** (ROM string, US#46 / EU#46): Frontend menus, Controller Pak UI, options ($Revision: 1.17; 'frontend.c'; 112 functions)
- **`trackrend.c`** (inferred name, US#47 / EU#47): 3D track/object display list builder ($Revision: 1.34, 1.5; 5 functions right before main)
- **`n64init.c`** (ROM string, US#48 / EU#48): main(), InitTvTiming(), N64 OS/VI/PI/audio boot ($Revision: 1.17; 'n64init.c'; 7 functions; 4B EU gap proves end)
- **`Particle.c`** (ROM string, US#49 / EU#49): Particle system ($Revision: 1.17; 'Particle.c'; 35 US / 36 EU fns — EU adds memory-guard canary func at 0x68E7C; 12B EU gap proves end)
- **`unk_6b780.c`** (inferred name, US#50 / EU#50): 2-function helper called 16x from effects.c
- **`hiscores.c`** (inferred name, US#51 / EU#51): High score / record tables & name entry ($Revision: 1.17, 7.27, 1.5; 'DIG'..'CJR', 'KLIES BRIDGE'..'VELOCITAR', 'NEW LAP RECORD')
- **`Racefunc.c`** (ROM string, US#52 / EU#52): Race completion, challenge medals, circuit progression ($Revision: 1.17; 'Racefunc.c'; 16 functions; 4B ZERO pad in EU)
- **`pause.c`** (inferred name, US#53 / EU#53): Pause menu & music track name table ($Revision: 1.17; 'OUTRO'..'PAUSED'..'YES'/'NO'; 5 functions)
- **`inspect.c`** (ROM string, US#54 / EU#54): AI damage/ship inspector + 7 stripped debug stubs ($Revision: 1.17, 7.27, 1.5; 'inspect.c'; 11 functions)
- **`sfx.c`** (inferred name, US#55 / EU#55): libmus SFX wrapper ($Revision: 1.17; 7 functions, 48 call sites)
- **`Trail.c`** (ROM string, US#56 / EU#6): Ship exhaust/ribbon trails ('Trail.c'; 9 functions; MOVED to early_game in EU)
- **`libgcc (_cmpdi2.o)`** (known lib, US#57 / EU#56): GCC 2.7.2 libgcc2 __cmpdi2 (64-bit signed compare; 0B pad)
- **`libgcc (_floatdidf.o)`** (known lib, US#58 / EU#57): GCC 2.7.2 libgcc2 __floatdidf (64-bit signed int to double)
- **`libgcc (_udivdi3.o)`** (known lib, US#59 / EU#58): GCC 2.7.2 libgcc2 __udivdi3 + __udivmoddi4 + 256B __clz_tab
- **`libmus (player.o)`** (known lib, US#60 / EU#59): Software Creations N64 libmus music player (82 functions; 8B ZERO pad at end)
- **`libmus (aud_thread/dma/sched)`** (known lib, US#61 / EU#60): N64 libmus audio thread/DMA/scheduler (7 functions; IDO-compiled)

## Europe `early_game` Link Order (`0x08830..0x10680`)

| EU# | US# | TU Name | EU `.rodata` ROM | EU `.text` ROM | EU `.text` Pad | US `.text` ROM |
|---:|---:|---|---|---|---|---|
| 5 | 39 | `effects.c` | 0x1100..0x11A0 | `0x08830..0x0E40C` | 4B @0x0E40C (heap) | `0x44710..0x4A550` |
| 6 | 56 | `Trail.c` | 0x11A0..0x11B0 | `0x0E410..0x10670` | 0B | `0x70CF0..0x72EE4` |
| 7 | 6 | `assert.c` | — | `0x10670..0x10678` | 8B @0x10678 (heap) | `0x09840..0x09848` |
| 8 | 7 | `audio.c` | 0x11B0..0x11F0 | `0x10680..0x10EF8` | 8B @0x10EF8 (heap) | `0x09850..0x0A0C8` |

## USA `.text` `psylink` Padding Gaps (45 gaps)

| # | After TU | ROM Range | VRAM Range | Size | Kind | Raw Bytes |
|---:|---|---|---|---:|---|---|
| 1 | `assert.c` | `0x09848..0x09850` | `0x80008C48..0x80008C50` | 8B | psylink_heap | `56578b7e0633f6e9` |
| 2 | `audio.c` | `0x0A0C8..0x0A0D0` | `0x800094C8..0x800094D0` | 8B | psylink_heap | `f80000c746f00000` |
| 3 | `gfx.c` | `0x0A8A4..0x0A8B0` | `0x80009CA4..0x80009CB0` | 12B | psylink_heap | `c45eec26837f02007503e9e5` |
| 4 | `Scene.c` | `0x0F51C..0x0F520` | `0x8000E91C..0x8000E920` | 4B | psylink_heap | `561e680f` |
| 5 | `files.c` | `0x0FD0C..0x0FD10` | `0x8000F10C..0x8000F110` | 4B | psylink_heap | `7efa0bd2` |
| 6 | `pad.c` | `0x103D4..0x103E0` | `0x8000F7D4..0x8000F7E0` | 12B | psylink_heap | `750433c0eb20ff7604ff76fe` |
| 7 | `Init.c` | `0x11AE8..0x11AF0` | `0x80010EE8..0x80010EF0` | 8B | psylink_heap | `0301a12102001300` |
| 8 | `decomp.c` | `0x11C94..0x11CA0` | `0x80011094..0x800110A0` | 12B | psylink_heap | `020013000301472902001200` |
| 9 | `Ships.c` | `0x1582C..0x15830` | `0x80014C2C..0x80014C30` | 4B | psylink_heap | `bab3594f` |
| 10 | `math_sfx.c` | `0x15C1C..0x15C20` | `0x8001501C..0x80015020` | 4B | psylink_heap | `9f96918a` |
| 11 | `Track.c` | `0x18FA8..0x18FB0` | `0x800183A8..0x800183B0` | 8B | psylink_heap | `2a2b32322a242525` |
| 12 | `Initrace.c` | `0x19C08..0x19C10` | `0x80019008..0x80019010` | 8B | psylink_heap | `4f4f4f4f594f4f4b` |
| 13 | `Wadfile.c` | `0x19F48..0x19F50` | `0x80019348..0x80019350` | 8B | psylink_heap | `59554f4f4f363636` |
| 14 | `unk_19f50.c` | `0x1A0E4..0x1A0F0` | `0x800194E4..0x800194F0` | 12B | psylink_heap | `793f3f242421252522212434` |
| 15 | `Skeleton.c` | `0x1AB8C..0x1AB90` | `0x80019F8C..0x80019F90` | 4B | psylink_heap | `324b574b` |
| 16 | `Radscn.c` | `0x1B804..0x1B810` | `0x8001AC04..0x8001AC10` | 12B | psylink_heap | `d8430109080143801243d893` |
| 17 | `camera.c` | `0x1D418..0x1D420` | `0x8001C818..0x8001C820` | 8B | psylink_heap | `3446324f52596060` |
| 18 | `utils.c` | `0x1DBCC..0x1DBD0` | `0x8001CFCC..0x8001CFD0` | 4B | psylink_heap | `2a32324b` |
| 19 | `Dynam.c` | `0x20308..0x20310` | `0x8001F708..0x8001F710` | 8B | psylink_heap | `01297894d8d81094` |
| 20 | `shipctrl.c` | `0x21834..0x21840` | `0x80020C34..0x80020C40` | 12B | psylink_heap | `010101010101010101010809` |
| 21 | `control.c` | `0x243A4..0x243B0` | `0x800237A4..0x800237B0` | 12B | psylink_heap | `080101010101010107090707` |
| 22 | `collide.c` | `0x27B78..0x27B80` | `0x80026F78..0x80026F80` | 8B | psylink_heap | `a55a4200ffff0000` |
| 23 | `shipefx.c` | `0x2871C..0x28720` | `0x80027B1C..0x80027B20` | 4B | psylink_heap | `636e636e` |
| 24 | `scnanim.c` | `0x2A388..0x2A390` | `0x80029788..0x80029790` | 8B | psylink_heap | `604f59594f4f4b32` |
| 25 | `Race.c` | `0x2AF1C..0x2AF20` | `0x8002A31C..0x8002A320` | 4B | psylink_heap | `a5cfcfab` |
| 26 | `hud.c` | `0x34E88..0x34E90` | `0x80034288..0x80034290` | 8B | psylink_heap | `2409010701010101` |
| 27 | `gfx2d.c` | `0x3624C..0x36250` | `0x8003564C..0x80035650` | 4B | psylink_heap | `01b7cbcd` |
| 28 | `font.c` | `0x374D4..0x374E0` | `0x800368D4..0x800368E0` | 12B | psylink_heap | `010101010101010101010101` |
| 29 | `weapon.c` | `0x44708..0x44710` | `0x80043B08..0x80043B10` | 8B | psylink_heap | `2222222222222222` |
| 30 | `cont.c` | `0x4AB5C..0x4AB60` | `0x80049F5C..0x80049F60` | 4B | psylink_heap | `fbfbfbfb` |
| 31 | `sched.c` | `0x4AC24..0x4AC30` | `0x8004A024..0x8004A030` | 12B | psylink_heap | `fbfbe4c1c6e2dfe4e7b18f9c` |
| 32 | `cpu.c` | `0x50258..0x50260` | `0x8004F658..0x8004F660` | 8B | psylink_heap | `fbfbfbfbfbfbfbfb` |
| 33 | `n64render.c` | `0x50738..0x50740` | `0x8004FB38..0x8004FB40` | 8B | psylink_heap | `7b83838383838383` |
| 34 | `unk_50740.c` | `0x50748..0x50750` | `0x8004FB48..0x8004FB50` | 8B | psylink_heap | `19191913cdf310bf` |
| 35 | `frontend.c` | `0x654E8..0x654F0` | `0x800648E8..0x800648F0` | 8B | psylink_heap | `fafafafafafafa00` |
| 36 | `unk_6b780.c` | `0x6B90C..0x6B910` | `0x8006AD0C..0x8006AD10` | 4B | psylink_heap | `35b9ff00` |
| 37 | `hiscores.c` | `0x6CD28..0x6CD30` | `0x8006C128..0x8006C130` | 8B | psylink_heap | `fa0dcacbf3c7cabb` |
| 38 | `Racefunc.c` | `0x6FB0C..0x6FB10` | `0x8006EF0C..0x8006EF10` | 4B | psylink_heap | `fafafafa` |
| 39 | `pause.c` | `0x70138..0x70140` | `0x8006F538..0x8006F540` | 8B | psylink_heap | `4aceff004ad6ff00` |
| 40 | `inspect.c` | `0x70848..0x70850` | `0x8006FC48..0x8006FC50` | 8B | psylink_heap | `d1a395873565b457` |
| 41 | `sfx.c` | `0x70CEC..0x70CF0` | `0x800700EC..0x800700F0` | 4B | psylink_heap | `a0958e82` |
| 42 | `Trail.c` | `0x72EE4..0x72EF0` | `0x800722E4..0x800722F0` | 12B | psylink_heap | `fafafafafa7884959aced0d3` |
| 43 | `libgcc (_floatdidf.o)` | `0x72FEC..0x72FF0` | `0x800723EC..0x800723F0` | 4B | psylink_heap | `fafafafa` |
| 44 | `libgcc (_udivdi3.o)` | `0x7364C..0x73650` | `0x80072A4C..0x80072A50` | 4B | psylink_heap | `8600186c` |
| 45 | `libmus (player.o)` | `0x76208..0x76210` | `0x80075608..0x80075610` | 8B | zero_pad | `0000000000000000` |

## Europe `.text` `psylink` Padding Gaps (47 gaps)

| # | After TU | ROM Range | VRAM Range | Size | Kind | Raw Bytes |
|---:|---|---|---|---:|---|---|
| 1 | `effects.c` | `0x0E40C..0x0E410` | `0x8000D80C..0x8000D810` | 4B | psylink_heap | `eaee402f` |
| 2 | `assert.c` | `0x10678..0x10680` | `0x8000FA78..0x8000FA80` | 8B | psylink_heap | `7b969c89ab006e4c` |
| 3 | `audio.c` | `0x10EF8..0x10F00` | `0x800102F8..0x80010300` | 8B | psylink_heap | `eb085b0e5770b2e0` |
| 4 | `gfx.c` | `0x116CC..0x116D0` | `0x80010ACC..0x80010AD0` | 4B | psylink_heap | `feb5cf91` |
| 5 | `Scene.c` | `0x1633C..0x16340` | `0x8001573C..0x80015740` | 4B | psylink_heap | `00120013` |
| 6 | `files.c` | `0x16B2C..0x16B30` | `0x80015F2C..0x80015F30` | 4B | psylink_heap | `d07d8439` |
| 7 | `pad.c` | `0x171F4..0x17200` | `0x800165F4..0x80016600` | 12B | psylink_heap | `7ab2869c88d1e66f0822228f` |
| 8 | `Init.c` | `0x18908..0x18910` | `0x80017D08..0x80017D10` | 8B | psylink_heap | `74e4c062d1b4170c` |
| 9 | `decomp.c` | `0x18AB4..0x18AC0` | `0x80017EB4..0x80017EC0` | 12B | psylink_heap | `69b702dfa0ba01669e0b0130` |
| 10 | `math_sfx.c` | `0x1CBCC..0x1CBD0` | `0x8001BFCC..0x8001BFD0` | 4B | psylink_heap | `01a90000` |
| 11 | `Track.c` | `0x1FEE8..0x1FEF0` | `0x8001F2E8..0x8001F2F0` | 8B | psylink_heap | `0001030ccdcdcdcd` |
| 12 | `Initrace.c` | `0x20B48..0x20B50` | `0x8001FF48..0x8001FF50` | 8B | psylink_heap | `000005d3cdcdcdcd` |
| 13 | `Wadfile.c` | `0x20E88..0x20E90` | `0x80020288..0x80020290` | 8B | psylink_heap | `ffffc861cdcdcdcd` |
| 14 | `unk_19f50.c` | `0x21024..0x21030` | `0x80020424..0x80020430` | 12B | psylink_heap | `00000182ffffa5fccdcdcdcd` |
| 15 | `Skeleton.c` | `0x21ACC..0x21AD0` | `0x80020ECC..0x80020ED0` | 4B | psylink_heap | `cdcdcdcd` |
| 16 | `Radscn.c` | `0x22744..0x22750` | `0x80021B44..0x80021B50` | 12B | psylink_heap | `000002d3fffe377dcdcdcdcd` |
| 17 | `utils.c` | `0x24BEC..0x24BF0` | `0x80023FEC..0x80023FF0` | 4B | psylink_heap | `27276700` |
| 18 | `Dynam.c` | `0x275A4..0x275B0` | `0x800269A4..0x800269B0` | 12B | psylink_heap | `2030a10003df03de03d903da` |
| 19 | `shipctrl.c` | `0x28B24..0x28B30` | `0x80027F24..0x80027F30` | 12B | psylink_heap | `05410540053b053c0000f000` |
| 20 | `control.c` | `0x2B848..0x2B850` | `0x8002AC48..0x8002AC50` | 8B | psylink_heap | `00009c3f000001eb` |
| 21 | `collide.c` | `0x2F21C..0x2F220` | `0x8002E61C..0x8002E620` | 4B | psylink_heap | `00a50001` |
| 22 | `shipefx.c` | `0x2FDBC..0x2FDC0` | `0x8002F1BC..0x8002F1C0` | 4B | psylink_heap | `000000a4` |
| 23 | `droid.c` | `0x30D14..0x30D20` | `0x80030114..0x80030120` | 12B | psylink_heap | `803096c0803096d4803096f4` |
| 24 | `scnanim.c` | `0x31A98..0x31AA0` | `0x80030E98..0x80030EA0` | 8B | psylink_heap | `80309e0c80309e30` |
| 25 | `Race.c` | `0x3262C..0x32630` | `0x80031A2C..0x80031A30` | 4B | psylink_heap | `8030a414` |
| 26 | `hud.c` | `0x3C57C..0x3C580` | `0x8003B97C..0x8003B980` | 4B | psylink_heap | `40080000` |
| 27 | `loader.c` | `0x3CCA8..0x3CCB0` | `0x8003C0A8..0x8003C0B0` | 8B | psylink_heap | `7cb87f2e67cbabf2` |
| 28 | `gfx2d.c` | `0x3D93C..0x3D940` | `0x8003CD3C..0x8003CD40` | 4B | psylink_heap | `0dd4cf85` |
| 29 | `font.c` | `0x3EBC4..0x3EBD0` | `0x8003DFC4..0x8003DFD0` | 12B | psylink_heap | `d6c14c104ed0c8b90303814c` |
| 30 | `weapon.c` | `0x4C42C..0x4C430` | `0x8004B82C..0x8004B830` | 4B | psylink_heap | `d4021019` |
| 31 | `cont.c` | `0x4CA3C..0x4CA40` | `0x8004BE3C..0x8004BE40` | 4B | psylink_heap | `851206ea` |
| 32 | `sched.c` | `0x4CB04..0x4CB10` | `0x8004BF04..0x8004BF10` | 12B | psylink_heap | `3a1b1f7fbb77bc9a0e061203` |
| 33 | `cpu.c` | `0x51FEC..0x51FF0` | `0x800513EC..0x800513F0` | 4B | psylink_heap | `ddcb6a1e` |
| 34 | `n64render.c` | `0x524C8..0x524D0` | `0x800518C8..0x800518D0` | 8B | psylink_heap | `77533d2992d4b887` |
| 35 | `unk_50740.c` | `0x524D8..0x524E0` | `0x800518D8..0x800518E0` | 8B | psylink_heap | `b5ecffbf8bdbfff7` |
| 36 | `frontend.c` | `0x6725C..0x67260` | `0x8006665C..0x80066660` | 4B | psylink_heap | `10000000` |
| 37 | `n64init.c` | `0x68E2C..0x68E30` | `0x8006822C..0x80068230` | 4B | psylink_heap | `403d6600` |
| 38 | `Particle.c` | `0x6D7C4..0x6D7D0` | `0x8006CBC4..0x8006CBD0` | 12B | psylink_heap | `00001d901000000000000000` |
| 39 | `unk_6b780.c` | `0x6D95C..0x6D960` | `0x8006CD5C..0x8006CD60` | 4B | psylink_heap | `42416600` |
| 40 | `hiscores.c` | `0x6ED88..0x6ED90` | `0x8006E188..0x8006E190` | 8B | psylink_heap | `000400090009000b` |
| 41 | `Racefunc.c` | `0x71AAC..0x71AB0` | `0x80070EAC..0x80070EB0` | 4B | zero_pad | `00000000` |
| 42 | `pause.c` | `0x720D8..0x720E0` | `0x800714D8..0x800714E0` | 8B | psylink_heap | `4d4d86008484af00` |
| 43 | `inspect.c` | `0x727E8..0x727F0` | `0x80071BE8..0x80071BF0` | 8B | psylink_heap | `1000000000000000` |
| 44 | `sfx.c` | `0x72C8C..0x72C90` | `0x8007208C..0x80072090` | 4B | psylink_heap | `033e0000` |
| 45 | `libgcc (_floatdidf.o)` | `0x72D8C..0x72D90` | `0x8007218C..0x80072190` | 4B | psylink_heap | `00001e0b` |
| 46 | `libgcc (_udivdi3.o)` | `0x733EC..0x733F0` | `0x800727EC..0x800727F0` | 4B | psylink_heap | `00060005` |
| 47 | `libmus (player.o)` | `0x75FA8..0x75FB0` | `0x800753A8..0x800753B0` | 8B | zero_pad | `0000000000000000` |
