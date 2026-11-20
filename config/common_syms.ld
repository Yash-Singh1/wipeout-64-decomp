/* N64 OS low-memory globals (fixed virtual addresses) */
osTvType = 0x80000300;
osRomType = 0x80000304;
osRomBase = 0x80000308;
osResetType = 0x8000030C;
osCicId = 0x80000310;
osVersion = 0x80000314;
osMemSize = 0x80000318;
osAppNMIBuffer = 0x8000031C;

/* Shared cartridge file-table relative offsets (identical in USA and Europe) */
_romFilesEnd = 0x003E8F10;
_trackFilesStart = 0x00027F80;
_startupImageFile = 0x003463A0;
_startupDataFile = 0x00355F90;
g_heapRegionEnd = 0x803B5000;
