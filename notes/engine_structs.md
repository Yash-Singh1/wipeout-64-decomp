# Wipeout 64 engine structs vs Wipeout XL / Wipeout 3

Field offsets below come from the retail USA disassembly, read straight from
`orig/us/baserom.z64`. I checked each key function against its retail Europe counterpart:
Europe uses identical struct offsets, and only relocations, `__LINE__` arguments and PAL
constants differ. The references are the PS1 XL headers
(`../wipeout-xl-ps1/game/include/wipeout.h`), the PC XL headers (`../include/`) and Wipeout 3
(`../wipeout-3/decomp/include/`). Wipeout 64 descends from XL: its layouts are XL's with
additions, not Wipeout 3's packed ones.

Two engine-wide facts explain most of the differences:

- **Half-scale world.** `LoadPrm` halves the embedded skeleton translation (`t[i] >>= 1`), and
  object and track vertices are halved when they are converted to N64 `Vtx`
  (`sll 16; sra 17`). The source data keeps PSX scale; the N64 copies are half size so they
  fit in `Vtx.ob` shorts.
- **Software GTE.** `SetRotMatrix` (USA `0x80014DBC`) and `SetTransMatrix` (`0x80014D94`) copy
  the matrix into a C-side "GTE" state at `0x800AAE30` (rotation, `0x14` bytes) and
  `0x800AAE44` (translation). `MulMatrix0` is C at `0x800194F0` (`>>12` fixed point).

## Key globals

| Global | USA | Europe | Proto | Evidence |
|---|---|---|---|---|
| `g_heap` (`HeapBlock *`) | `0x80099610` | `0x800A6BF0` | `0x8009A720` | First argument of almost every `DAlloc` |
| heap whose free space `lowTideMark` tracks | `0x80099614` | `0x800A6BF4` | `0x8009A724` | Compared in `DAlloc` |
| `lowTideMark` | `0x8008A848` | `0x80089E78` | `0x8008B958` | `if (heap == X && heap->free < lowTideMark) lowTideMark = free` |
| `ships` (`ShipData *`) | `0x800AA0A4` | `0x800B767C` | `0x800AB1B4` | Indexed `i*0x108` |
| `shipShapes[]` (`Object *`) | `0x8009BA68` | `0x800A9048` | `0x8009CB78` | `shipShapes[ship->lowResTableIndex]` in Ships.c |
| `track` (`TrackData *`) | `0x8008A98C` | `0x80089FBC` | `0x8008BA9C` | Track.c `Init*` functions |
| `frontendOptions_g` | `0x800C5B20` | `0x800C5E60` | `0x800C6C30` | frontend.c asserts |
| `ownShip`; `ownShip2`..`ownShip4` | `0x800A5E2C`; `0x800A9F8C`/`90`/`94` | `0x800B3414`; ... | `0x800A6F3C` | Set from `frontendOptions_g.ownShip[]` |
| sine table (`short[4096]`, scale 16384) | `0x8008AEB0` | `0x8008A4E0` | `0x8008BFC0` | Skeleton.c; `cos(a) = table[(a & 0xFFF) + 1024]` |

## `MATRIX` / `Skeleton` (Skeleton.c, USA `0x80019780..0x80019F8C`)

`DAlloc(heap, 0x50, "Skeleton.c", 42)` allocates a skeleton. The layout is identical to XL
(and to Wipeout 3):

| Offset | Field | Type | XL | Evidence |
|---|---|---|---|---|
| `0x00` | `relative.m[3][3]` | `short` | same | `NewSkeleton` sets `0x00`/`0x08`/`0x10` to `0x1000` |
| `0x12` | (pad) | | same | |
| `0x14` | `relative.t[3]` | `long` | same | `SetSkeletonPosition` |
| `0x20` | `absolute` | `MATRIX` | same | `GetSkeletonMatrix` returns `skel+0x20` |
| `0x40` | `update` | `short` | same | Set to 1 by the setters, cleared by `GetSkeletonMatrix` |
| `0x44` | `super` | `Skeleton *` | same | Recursion target; track skeletons set it to the world root at `0x800CBAF0` |
| `0x48` | `sub` | `Skeleton *` | same | Zeroed by `NewSkeleton` |
| `0x4C` | `next` | `Skeleton *` | same | Zeroed by `NewSkeleton` |

`MATRIX` is the PsyQ layout (`short m[3][3]; long t[3]`, `0x20` bytes).

Functions, with USA / Europe / proto addresses:

| Name | USA | Europe | Proto | Notes |
|---|---|---|---|---|
| `NewSkeleton` | `0x80019780` | `0x800206C0` | `0x80019970` | Proper identity (`m[2][2] = 4096`); the PC XL build's `m[2][1]` quirk is absent |
| `SetSkeletonPosition` | `0x80019808` | `0x80020748` | `0x800199F8` | |
| `SetSkeletonDirection` | `0x80019820` | `0x80020760` | `0x80019A10` | Inline table sine (`>>16` products, `-s >> 2`) instead of `rsin`/`rcos` |
| `SetSkeletonDirectionHPR` | `0x800199B0` | `0x800208F0` | `0x80019BA0` | Identified by `m[1][2] = -sin(arg2)` |
| `SetSkeletonDirectionRPH` | `0x80019B44` | `0x80020A84` | `0x80019D34` | Identified by `m[2][1] = sin(arg2)`. There is no `HRP` variant |
| `GetSkeletonMatrix` | `0x80019CE0` | `0x80020C20` | `0x80019ED0` | Returns `&relative` when `super == NULL`, else updates and returns `&absolute`, as in XL |
| (unnamed) `MATRIX` to `Mtx` | `0x80019E2C` | `0x80020D6C` | `0x8001A01C` | N64 only, see below |
| `SetSkeletonTransform` | `0x80019F54` | `0x80020E94` | `0x8001A144` | Calls `SetTransMatrix` then `SetRotMatrix`, the same order as XL |

The `MATRIX` to `Mtx` function `(MATRIX *src, Mtx *dst)` builds an N64 16.16 matrix from
`m << 4`, transposed. It negates the third row (a Z flip), takes integer translation from
the low halves of `t[0]`/`t[1]` and uses `-t[2]` with `w = 1`.

## `HeapBlock` / `DAlloc` (USA `0x8000E920..0x8000ED50`)

The XL `HeapBlock` is `0x14` bytes. Wipeout 64 extends it to `0x20` bytes with debug
tagging; blocks and data are 16-byte aligned.

| Offset | Field | XL | Evidence |
|---|---|---|---|
| `0x00` | `flags` (1 = in use) | same | |
| `0x04` | `size` (data bytes) | same | |
| `0x08` | `free` (bytes left; meaningful on the root block) | same | |
| `0x0C` | `data` | same | `DAlloc` returns `block->data` |
| `0x10` | `next` | same | |
| `0x14` | `magic` = `0x12345678` | **new** | Written by the init and alloc paths, checked by `0x8000E954` |
| `0x18` | `line` (`__LINE__` of the caller) | **new** | |
| `0x1C` | file id (index into the file-name table) | **new** | Returned by `0x8000EB5C(file)` |

The DAlloc signature is `void *DAlloc(HeapBlock *heap, long size, const char *file, int line)`.
Every caller passes `"<file>.c"` and `__LINE__`, which is the source of the line-number
table in `proto_clues.md`.

| USA | Europe | Role |
|---|---|---|
| `0x8000E920` | `0x80015740` | `InitDynamicMem(heap, size)`: root `size = free = size - 0x20`, `data = (heap + 0x2F) & ~0xF`, sets the magic |
| `0x8000E954` | `0x80015774` | Heap check: walks the list, and on a bad magic dereferences `NULL->next` to crash on purpose |
| `0x8000E990` | `0x800157B0` | `DAlloc`: size rounded up to 16; exact-fit reuse of a free block, else the smallest free block `>= size + 0x20` (and `< 0x800000`) is split from its tail; updates `heap->free` and `lowTideMark` |
| `0x8000EB18` | `0x80015938` | `memcmp`-like helper `(a, b, n)` returning 0 or 1; also used by `LoadPrm` to compare object names |
| `0x8000EB5C` | `0x8001597C` | File-name registry: a table of 12-byte entries at `0x80099620` (USA) holding names cut at `'.'`, with a `u8` count at `0x8008A840`. Returns the index |
| `0x8000EC94` | `0x80015AB4` | `DFree(heap, ptr)`: merges with a free next block, then with the previous block if that is free |

Wipeout 3 has the same `lowTideMark` global but keeps the 5-word header.

## `Object` and `Prm` (Scene.c, USA `0x80009CB0..0x8000E920`)

`LoadPrm` is at USA `0x80009E70` (Europe `0x80010C90`). Its arguments are
`(WADFILE *wad, <file record whose first word is the name>, <texture info>)`; `texture_info+6`
is the texture start that gets added to sprite textures. Each file record is `Object`
(`0x40` bytes), then `Skeleton` (`0x50` bytes), then vertices, normals and primitives.
The `Object` layout equals the PS1 XL one (`0x40` bytes); two slots have N64 meanings:

| Offset | Field | PS1 XL | Wipeout 64 use |
|---|---|---|---|
| `0x00` | `name[16]` | same | Compared against `"rot"`, `"fannybasher"`, `"C_CART_1"` and similar names for per-track animated objects |
| `0x10` | `vertexCount` (`short`) | same | |
| `0x14` | `vertices` (`SVECTOR *`) | same | |
| `0x18` | `normalCount` (`short`) | same | `> 0` triggers `0x80009D04` (normal processing) |
| `0x1C` | `normals` (`SVECTOR *`) | same | |
| `0x20` | `primitiveCount` (`short`) | same | |
| `0x24` | `primitives` | same | |
| `0x28` | XL `scratch` | `int` | **`Vtx *`**: built by `0x8000A4A8` with `DAlloc(vertexCount * 16)`; `ob = v >> 1`, `flag = 0`, `tc = (-1, -1)`, `cn = (0, 0, 0, 1)` |
| `0x2C` | XL `bspTree` | `BspTree *` | **Display-list group table**: `DAlloc(8)` of `{int count; entry *list}` with 8-byte entries holding a `Gfx *` at `+4` (Ships.c reads it for ship models) |
| `0x30` | `skeleton` | same | Points into the file image; its `t[]` is halved |
| `0x34` | `extent` | same | |
| `0x38` | `flags` (`short`) | same | |
| `0x3A` | `index` (`short`) | same | |
| `0x3C` | `next` | same | Set to 0, then linked to the previous object |

Wipeout 3's `Object` is a different, packed `0x28`-byte record; don't use it here.

Primitive strides in `LoadPrm` (jump table at USA `0x800006B8`, indexed by `type - 1`):

| Type | Stride | XL `sizeof` | | Type | Stride | XL `sizeof` |
|---|---|---|---|---|---|---|
| 1 `F3` | `0x10` | `0x10` | | 12 `LSF3` | `0x10` | `0x10` |
| 2 `FT3` | `0x1C` | `0x1C` | | 13 `LSFT3` | **`0x1E`** | `0x1C` |
| 3 `F4` | `0x10` | `0x10` | | 14 `LSF4` | `0x14` | `0x14` |
| 4 `FT4` | `0x20` | `0x20` | | 15 `LSFT4` | **`0x22`** | `0x20` |
| 5 `G3` | `0x18` | `0x18` | | 16 `LSG3` | `0x1C` | `0x1C` |
| 6 `GT3` | `0x24` | `0x24` | | 17 `LSGT3` | **`0x2A`** | `0x24` |
| 7 `G4` | `0x1C` | `0x1C` | | 18 `LSG4` | `0x24` | `0x24` |
| 8 `GT4` | `0x2C` | `0x2C` | | 19 `LSGT4` | `0x34` | `0x34` |
| 9 `LF2` | error | | | 20 `Spline` | `0x38` | `0x38` |
| 10 `TSPR`, 11 `BSPR` | `0x10` (`texture` at `+0x0A` gets `+= textureStart`) | `0x10` | | 21 `InfiniteLight` | `0x10` | `0x10` |
| | | | | 22 `PointLight` | `0x1C` | `0x1C` |
| | | | | 23 `SpotLight` | `0x28` | `0x28` |

The three bold strides are larger than the XL structs. Either Wipeout 64's PRM records add
fields there, or the file format pads them; the field layout inside them is not verified
yet. Untextured types (F3, F4, G3, G4 and the LS untextured types) are counted as they are
walked, which feeds display-list sizing. An unknown type prints
`"Bad primitive type %x in AdvancePoly"` (`AdvancePoly` is USA `0x8000DAA4`). After the walk,
`0x8000AAC4` groups primitives by texture (helper `0x8000D9F0`) and `0x8000AC00`/`0x8000ADD0`
build the display lists.

## `ShipData` (Ships.c / Dynam.c)

Ships are indexed `ships[i]` with `(i*33) << 3`, a **`0x108`-byte stride**. PS1 XL's
`ShipData` is `0xF0` bytes. Wipeout 64 inserts 4 bytes at `+0x04` and 20 bytes before
`update`, so XL fields from `nearTrkSect` to `startY` sit at XL offset + 4. Races have
15 ships (`i < 15` loops).

| W64 | XL | Field | Evidence |
|---|---|---|---|
| `0x00` | `0x00` | `highResTableIndex` (`short`) | |
| `0x02` | `0x02` | `lowResTableIndex` (`short`) | `shipShapes[ship->lowResTableIndex]` in Ships.c `0x800146CC` |
| `0x04` | | **new, 4 bytes** (role unknown) | |
| `0x08` | `0x04` | `nearTrkSect` | `->nextSection.ptr->centre - ->centre` in the position sort `0x8001FB60` |
| `0x0C` | `0x08` | `prevShipSect` | `prevShipSect = nearTrkSect` (`0x8001F1D8`, `0x80020C70`) |
| `0x10` | `0x0C` | `attr` (`int`) | The most-used word |
| `0x14`/`0x24`/`0x34` | `0x10`/`0x20`/`0x30` | `unitVecNose`/`unitVecWing`/`unitVecFin` | |
| `0x44`/`0x54`/`0x64` | `0x40`/`0x50`/`0x60` | `ppivot`/`vpivot`/`apivot` | `ppivot - section->centre` in the sort |
| `0x74..0x82` | `0x70..0x7E` | `hdg`, `pitch`, `roll`, `vhdg`, `vpitch`, `vroll`, `ahdg`, `mass` | |
| `0x84` | `0x80` | `thrustVector` | |
| `0x94`/`0x96` | `0x90`/`0x92` | `r_brake_rot`/`l_brake_rot` | |
| `0x98` | `0x94` | `speed` (`u16`) | |
| `0x9C` | `0x98` | `thrust` | |
| `0xB0` | `0xAC` | `combatAttr` (`u16`) | `& 0x8000` (XL sets `combatAttr \|= 0x8000`) |
| `0xBE` | `0xBA` | `weapon` (`u8`) | |
| `0xCC`/`0xCE` | `0xC8`/`0xCA` | `nearSecNo`/`prevSecNo` | Loaded from `section->secNo` (`+0x98`) |
| `0xD0..0xE3` | `0xCC` | `lapTimes[5]` | Inferred from the shift |
| `0xE4` | `0xE0` | `updateCount` | `== 200` (USA) or `166` (PAL) in `0x80020C74` |
| `0xEC` | `0xE8` | `startY` | `= ppivot.vy` when `updateCount` hits the limit |
| `0xF0` | | **new** `int` counter | `ships[weapon->owner].0xF0++` in `0x80042EF0` (likely hits or eliminations) |
| `0xF4` | | **new** `u8` | Read by frontend.c `0x80056B08` and Trail.c |
| `0xFC` | | **new** `short` | Early-out test in `0x800427A0` |
| `0xFE` | | **new** `u8` | |
| `0x104` | `0xEC` | `update` (function pointer) | Swapped between `0x8001FF04`, `0x80020C40` and `0x80020F30` in Racefunc.c `0x8006E4F0` |

Open: what the new fields at `0x04` and `0xF0..0x103` are, and whether `0xF0..0x103`
continues the extra fields (`0xF8`/`0x100` are untouched in the ship-pointer scan).

## `TrackSection` / `TrackData` (Track.c)

`TrackSection` is the XL layout, **`0x9C` bytes** (section loops step by `0x9C`):
`junction`/`prevSection`/`nextSection` at `0x00`/`0x04`/`0x08`, `centre` at `0x0C`,
`faceStart` at `0x8E`, `flags` at `0x96` and `secNo` at `0x98`, all confirmed. Wipeout 3
repacked it to `0x40` bytes, so it is not a useful reference here. `Face` is the XL
`0x14`-byte record (`red`/`green`/`blue` at `0x10..0x12` go through a colour function at USA
`0x8004F63C` during `InitFaces`).

`TrackData` (pointer `track`) grew from XL's 8 words to at least `0x35C` bytes:

| W64 | XL | Field | Evidence |
|---|---|---|---|
| `0x00` | `0x00` | `vertexCount` | File size / 16 in `InitVertices` (`0x80015CA0`) |
| `0x04` | `0x04` | `faceCount` | `InitFaces` (`0x80016144`) |
| `0x08` | `0x08` | `sectionCount` | |
| `0x0C` | | **new** count of `Vtx` entries at `0x350` | `0x80015C14` clears `Vtx.flag` for them |
| `0x10` | `0x0C` | `vertices` (`VECTOR *`) | |
| `0x14`, `0x18` | | **new** `Vtx *` double buffer | `DAlloc((vertexCount + 0x600) * 16)` each, with the count `+0x600` stored at `0x800AAE78`; `ob = v >> 1`, `flag = 0xFFFF` initially |
| `0x1C` | `0x10` | `faces` (`Face *`) | |
| `0x20` | `0x14` | `sections` (`TrackSection *`) | `InitSections` (`0x80016340`) |
| `0x24` | `0x18` | (not touched by Track.c; XL `viewList` moved) | |
| `0x28` | | **new** `Skeleton **` per section (`sectionCount` pointers) | Filled with shared track skeletons |
| `0x2C` | | **new** skeleton count | Capped at 200 (`0xC8`) in `0x80015630` |
| `0x30` | | **new** `Skeleton *skeletons[200]` (to `0x34F`) | `0x80015630` calls `NewSkeleton` and clears `t`; every entry's `super` is set to `0x800CBAF0` |
| `0x350` | | **new** `Vtx *` | |
| `0x354` | `0x18` | `viewList` | `InitViewList` (`0x80016590`) |
| `0x358` | `0x1C` | `colCopy` | `InitFaces` copy of the colours |

## `frontendOptions_g` (frontend.c)

This struct has no counterpart in either sibling decomp; it is new in Wipeout 64. Base is
USA `0x800C5B20`, Europe `0x800C5E60`, proto `0x800C6C30`. Fields proven by the assert
function at USA `0x80052E40` (Europe `0x80054BC8`; asserts at lines 1943-1966):

| Offset | Field | Type | Evidence |
|---|---|---|---|
| `0x00` | `team` | `int` | `team = ownShip[0]`; `assert(team < 15)` |
| `0x04` | `teamClass` | `u8` | `assert(teamClass <= 3)` |
| `0x0C` | track or scene number | `int` | `LoadPrm` switches on 1..7; frontend code checks `< 7` |
| `0x10` | `numPlayers` | `u8` | `assert(numPlayers <= 4)`; switch on 1..4 |
| `0x15` | mode-like byte | `u8` | Read 50 times, mostly compared with 2 (game type?) |
| `0x16` | `u8` | | Heavily read in hud and frontend code |
| `0x18` | `ownShip[4]` | `int` | `assert(ownShip[i-1] != -1)`; copied to `ownShip`..`ownShip4` |
| `0x28` | `screenLayout[4]` | `int` | `assert(screenLayout[i-1] != -1)`; `[0] = 0` for one player |
| `0x38` | `int` | | Zeroed for one player; read in Dynam.c `0x8001F710` along with `0x3C`/`0x40`/`0x44` (probably a second per-player `[4]` array) |

Further offsets (`0x58..0x7C`, `0x64` as an `int` read 33 times, `0x68`/`0x69`, `0x6C`,
`0x74`/`0x78` used by audio.c) fall in the same block. Whether they belong to
`frontendOptions_g` or to neighbouring globals is not proven; confirm the struct's size
before declaring them.

## Method and caveats

- Function boundaries come from `jal` targets and function pointers in the data, not from
  splat, so a few static leaf functions are merged into their neighbours.
- Ship and track offsets were collected by tracking every register derived from the
  `ships`/`track` global and recording loads and stores through it. Fields marked
  "inferred" fit the shift but have no direct access.
- File labels for functions are the nearest `"<file>.c"` string, so they are approximate
  where a file has no strings.
