# Wipeout 64 (Europe) prototype: reverse-engineering clues

Source ROM: `orig/proto/Wipeout 64 (Europe) (Proto).n64` (16 MiB flash-cart dump). Only the
first 8 MiB are Wipeout 64. Compared against retail USA (`orig/us/baserom.z64`, `NWPE`) and
retail Europe (`orig/eu/baserom.z64`, `NWPP`). All addresses are VRAM unless marked ROM.

## Summary

The prototype is the USA code base built for PAL. Its source matches retail USA almost
everywhere: identical `.rodata` strings, asserts, `$Revision:` tags and `__LINE__` numbers.
It is not retail Europe. On top of USA it has the PAL work that shipped in retail Europe:
timing constants rescaled from 60 Hz to 50 Hz, extra `*60/50` integer conversions, PAL
`static_gfx`, PAL data tables and the `NWPP` save-game code. It also has the newer
`Particle.c` with a memory-guard canary. Retail Europe then changed more: a different link
order, larger buffers, `Track.c`/`n64init.c`/`Trail.c` edits and dropped a debug string.

Use the prototype to sort retail Europe differences into two kinds:

- The proto equals EU but USA differs: a deliberate PAL change. This is the usual
  `#ifdef VERSION_EU` case.
- The proto equals USA but EU differs: a late Europe-only fix or a relinked object.

## File format (correction)

The `.n64` file is **already big-endian (`.z64` order)**. Its header starts `80 37 12 40` and
the title reads `Wipeout 64`. Applying the 4-byte word reversal
(`b[0::4],b[1::4],b[2::4],b[3::4] = d[3::4],d[2::4],d[1::4],d[0::4]`) corrupts it: the header
becomes `40 12 37 80`. `tools/disasm.py -V proto` handles this correctly because it only
swaps when the header reads `40 12 37 80`. Read the file raw.

| Header field | Proto | USA | Europe |
|---|---|---|---|
| Game code (0x3B) | `NWPP` | `NWPE` | `NWPP` |
| CRC1 / CRC2 | `136EBEE3` / `35B7229E` | `132D2732` / `C70E9118` | `54310E7D` / `6B5430D8` |
| Entry, clock, libultra (0x08, 0x04, 0x0C) | same as retail | `80000400`, `0000000F`, `00001444` | same |

The proto's `0xFF` padding starts at ROM `0x7B7F20`. Data from `0x800000` on belongs to a
Chameleon Twist prototype (see `orig/proto/readmeh.txt`) and is not Wipeout 64.

## ROM layout

| Segment | Proto ROM | USA ROM | Europe ROM | Notes |
|---|---|---|---|---|
| `crt0` | `0x1000` | `0x1000` | `0x1000` | Same code; the proto clears `.bss` from `0x80099800` to `0x801001F0` |
| `.rodata` | `0x10F4..0x5660` | `0x10F4..0x5660` | `0x10F4..0x5640` | Proto strings equal USA; the 2306 differing bytes are jump tables and pointers |
| RSP text | `0x5660..0x9840` | `0x5660..0x9840` | `0x5640..0x8830` | Proto keeps `gspL3DEX_fifo` text at `0x7980`, as USA does |
| Game `.text` | `0x9840..0x77DB0` | `0x9840..0x76CA0` | `0x8830..0x76A40` | Proto is `0x1110` bytes larger than USA |
| libultra `.text` | `0x77DB0..0x89A20` | `0x76CA0..0x88910` | `0x76A40..0x886B0` | Identical in all three after relocation masking |
| RSP data | `0x89A20..0x8B060` | `0x88910..0x89F50` | `0x886B0..0x894F0` | Proto keeps L3DEX data at `0x8A4E0` |
| Game `.data` | `0x8B060..0x9A400` | `0x89F50..0x992F0` | `0x894F0..0x98890` | Proto and USA are both `0xF3A0` bytes |
| `.bss` (VRAM) | `0x80099800..0x801001F0` | `0x800986F0..0x800FF0E0` | `0x80097C90..0x800F7FF0` | Proto and USA are both `0x669F0` bytes |
| `static_gfx` | `0x9A400..0x9A650` | `0x99300..0x99550` | `0x988C0..0x98B10` | Proto bytes equal **Europe** (the PAL viewport and scissor) |
| `assets` | `0x9A650..0x7B7F20` | `0x99550..0x7B6E20` | `0x98B10..0x7B63E0` | Byte-identical in all three |

The proto has no `code_pad` gap: `.data` ends exactly where `static_gfx` starts.

Address conversion: proto game code equals USA addresses up to `0x80012760`, the first
changed function. After that the proto drifts gradually to `+0x1110`. Proto `.data` and
`.bss` symbols are USA `+0x1110` (for example `g_heap` `0x80099610` becomes `0x8009A720`).
Retail Europe's `.bss` is laid out differently; see the table in `engine_structs.md`.

## Strings, asserts and `$Revision:` tags

- **Strings.** Compared as NUL-delimited strings, proto `.rodata` equals USA `.rodata`, apart
  from alignment-fill bytes (`psylink` filled gaps with host memory). The proto adds no
  debug, assert or `printf` text, so it is not a debug build.
- **`$Revision:` tags.** `1.17`, `7.27`, `1.5` and `1.34` appear in all three builds in the
  same pattern. They come from shared headers compiled into many files, not from the
  `.c` files themselves.
- **Retail Europe vs USA and proto.** Europe drops `"Convert track with track10"` (Ships.c,
  USA `0x80015020`, a function Europe no longer has). It adds a second `"PRESS START"` and
  places the `Trail.c` string early (`0x800011A0`) because it links Trail.c early.
- **File names** found in strings: `audio.c`, `gfx.c`, `Scene.c`, `files.c`, `Error.c`,
  `Init.c`, `Ships.c`, `Track.c`, `Initrace.c`, `Wadfile.c`, `Skeleton.c`, `Radius.c`/`Radscn.c`,
  `Dynam.c`, `collide.c`, `Race.c`, `hud.c`, `loader.c`, `Tim.c`, `gfx2d.c`, `cpu.c`,
  `n64render.c`, `plasma.c`, `frontend.c`, `n64init.c`, `Particle.c`, `Racefunc.c`,
  `inspect.c`, `Trail.c`.

### `__LINE__` numbers (`DAlloc(heap, size, file, line)` and `__assert(expr, file, line)`)

All 135 call sites were extracted from each ROM. Every line number is identical across
the three builds except these:

| File | Call | Proto | USA | Europe | Reading |
|---|---|---|---|---|---|
| `Track.c` | 11 DAllocs (677 ... 1419) | same as USA | 677 ... 1419 | +1 each | Europe added one line before line 677 |
| `cpu.c` | DAlloc | 3253 | 3258 | 3256 | Three different revisions |
| `n64init.c` | assert `retVal != -1`, 2 DAllocs | 544, 705, 706 | 544, 705, 706 | 549, 710, 711 | Europe added 5 lines before 544 |
| `Particle.c` | DAlloc, assert `frame >= 0` | **123, 465** | 119, 447 | **123, 465** | The proto has Europe's Particle.c |
| `Trail.c` | DAlloc `0xAE60` | 466 | 458 | 451 | Proto newest; Europe trimmed lines |

Sizes also differ in two places. The `Particle.c` pool is `0xA800` in the proto and USA
but `0xE800` in Europe, which enlarged it. The bound in the `loader.c`/`gfx2d.c` assert
`((u32)glist2D-(u32)&i->dp) < (GLIST_LEN * sizeof(Gfx))` is `0x4400` in the proto and USA but
`0x4800` in Europe (USA `0x80034290`, `0x80034D64`, `0x80035240`).

Full call list (USA line numbers, which also apply to the proto except where noted above):
`audio.c` 119/125/454-462, `gfx.c` 90, `Scene.c` 258-2561 (asserts `vCount` 1003, `pCount`
1007, `size` 1040, `rData->dlist` 1042), `files.c` 230/545, `Error.c` 78, `Init.c`
466-940, `Ships.c` 2821-2941, `Initrace.c` 121, `Wadfile.c` 606/766, `Skeleton.c` 42,
`Radscn.c` 264/316/503, `Dynam.c` 1282, `collide.c` 381/1316/1317, `Race.c` 1706-2340,
`hud.c` 1396/2048/2249, `loader.c` 145, `Tim.c` 181/210, `gfx2d.c` 332/432, `n64render.c`
37/41, `plasma.c` 356-359, `frontend.c` 1401-12808, `Racefunc.c` 1811/1829, `inspect.c`
744/795.

## Code differences

Method: functions were split at `jal` targets and data function pointers (624 in the proto,
623 in USA, 622 in Europe). Bodies were compared after masking `jal`/`j` targets and every
`%lo` paired with a `lui`, then aligned in sequence. Relocation noise from Europe's
different link layout is still possible in the Europe column.

| Class | Count | Meaning |
|---|---|---|
| proto = USA = Europe | 486 | Unchanged |
| proto = Europe, USA differs | 30 | PAL changes that shipped in Europe |
| proto = USA, Europe differs | 46 | Europe-only changes or relinked code |
| all three differ | 60 | Mostly PAL changes plus further Europe edits (some relocation noise) |
| USA = Europe, proto differs | 1 | `collide.c` USA `0x80028AE4`: the proto version has a `0x58` frame (retail `0x60`) |

### PAL timing (60 Hz to 50 Hz) in the proto, also in Europe

Frame counts were rescaled by 5/6, with USA addresses:

- `Init.c` `0x80012760`: `200` becomes `166`, plus new `*60/50` scaling.
- `Initrace.c` `0x80018880`: `0x708` (1800) becomes `0x5DC` (1500).
- `Dynam.c` `0x8001FB60`: `0xC7` becomes `0xA5`, `0x5A` becomes `0x4B`, `0x4B0` (1200) becomes
  `0x3E8` (1000).
- `Dynam.c` `0x80020C70`: `updateCount == 200` becomes `166`.
- `Race.c`/`hud.c` region:
  - `0x80027228`: `0x3C` (60) becomes `0x32` (50).
  - `0x80036A40`: 40/30/24/120/150 become 33/25/20/100/125.
  - `0x80042764`: 6/60 become 5/50.
  - `0x8004A660`: `0xE10` (3600) becomes `0xBB8` (3000).
- `frontend.c`:
  - `0x80052C60` and `0x8005BE30`: `0x384` (900) becomes `0x2EE` (750).
  - `0x8005C388`: 30 becomes 25.
- Particle code `0x800686F4`: 30 becomes 25.
- `Racefunc.c` `0x8006EA10`: 200 becomes 166.

Elsewhere the PAL code converts time with integer `x*60/50`: `mult` by `0x51EB851F` with
`sra 4`, after `x*60` built from shifts. These conversions are new in the proto in
`Init.c 0x80012760`, `Radscn.c 0x8001ACAC`/`0x8001CFAC`/`0x8001DB1C`, `collide.c 0x800248D8`/
`0x80027648`, and the large hud and race function at `0x800388E0`. Use this pattern to spot
`VERSION_EU` blocks.

### Screen and layout constants

- `Race.c` `0x8002AA38`: viewport and scissor values change (`0x2FD` becomes `0x329`, `0x3FD`
  becomes `0x415`, `0x16F` becomes `0x167`, `0x308` becomes `0x342`).
- `hud.c` `0x8002D350`: text positions change (`0x4C` becomes `0x50`, `0x37A` becomes `0x38A`,
  `0xF5` becomes `0xFB`).
- `frontend.c` `0x8005AC14`: y limits change (`0x1C0` becomes `0x170`, `0x1DE`/`0x1DD` become
  `0x18E`/`0x18D`).
- `0x80049094`: `0x1E4` becomes `0x1E0` and `0x50` becomes `0x28`.
- `0x8004370C`: colour and position tables (40/60/210/250 become 0/0/200/200).
- `0x8002704C`: `±0x20` becomes `±0x26`.

### Memory-guard canary (proto and Europe, not USA)

The proto adds a guard check, a leaf function at proto `0x8006743C` (Europe `0x8006827C`):

```c
if (guard[0] != 0xA5A55A5A) for (;;);
if (guard[1] != 0x5A5AA5A5) for (;;);
```

The guard words are `.data` at proto `0x80097AF8` (Europe `0x80095F98`; USA has two zero words
at `0x800969E8`). They sit right after a six-word array initialised to `-1` (proto
`0x80097AE0`, used by hud.c and Racefunc.c) and right before Particle.c's statics. The check
runs at the end of three Particle.c routines, after they write into a 32-entry table of
`0x128`-byte records (USA `0x800E9938`) indexed by a counter at USA `0x800969F8`. The proto
and Europe Particle.c is 4 lines longer before its first DAlloc (line 119 becomes 123).
Europe also enlarged the particle pool from `0xA800` to `0xE800`.

### `.data` tables (USA vs proto; Europe equals the proto)

- USA `0x80091B00`: PAL timers `1200, 1200, 600, 600` become `1000, 1000, 500, 500`.
- USA `0x80091B28..0x80091B64`: two tables scaled by 6/5 (`1200, 1600, 2100, 2600` become
  `1440, 1920, 2520, 3120`, and `100, 120, 140, 160` become `120, 144, 168, 192`). These are
  per-frame rates rather than frame counts.
- USA `0x80094D28..0x80094E30`: float challenge target times lowered (for example `1950.0`
  becomes `1675.0` and `2160.0` becomes `1850.0`; 19 entries).
- USA `0x800950B8`/`0x8009510C`: `900` becomes `750`; `0x800950EC`: `480` becomes `400`;
  `0x8009513C`: `300` becomes `250`.
- USA `0x800923D8`: Controller Pak game code `"NWPE"` becomes `"NWPP"`.

### Link order: the readme's `0x80071180` claim

Proto `0x80071180` (`0x8C0` bytes) equals USA `0x800700F0` byte for byte, and it is not
missing from retail Europe. Europe moved it to `0x8000D810`. Retail Europe links two code
ranges before `__assert` (its `early_game` block, `0x80007C30..0x8000FA70`):

- USA `0x80043B10..0x8004986C`, the code that references the `sroid`/`light` objects.
- USA `0x800700F0..0x800721A0`, which includes `Trail.c`.

The proto keeps USA's order. The proto-only alignment against USA therefore applies to
Europe only after this reordering is taken into account.

### Other Europe-only changes (proto equals USA)

- `Tim.c` `LoadTexture` (USA `0x80034658`): 170 instructions differ even though the length
  is the same. The index arithmetic changes (`sll 5`/`sll 4`/`subu` becomes `sll 9`/`sll 3`), so
  a texture record stride probably changed. Not analysed further.
- `main.c`/`InitTvTiming` (USA `0x80065C10`), `n64init.c`, `Track.c`
  (`InitSections`/`InitViewList` sizes), and USA `0x8004C148..0x8004F3A8`/`0x80052334`/
  `0x8006C130` differ in length.
- `"Convert track with track10"` (USA `0x80015020`) is absent in Europe.

## Practical use

- When a USA function and its Europe counterpart differ, compare the proto. If the proto
  equals Europe, write a `#ifdef VERSION_EU` around the PAL constant or conversion. If the
  proto equals USA, the Europe change is late and the USA shape is the original.
- The proto's `Particle.c` is a third data point for the canary code: the same source with
  USA-style link addresses.
- Proto names agree with USA, because the strings and line numbers match. No new symbol
  names come from the proto.
