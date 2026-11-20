#ifndef WIPEOUT64_WADFILE_H
#define WIPEOUT64_WADFILE_H

#include "common.h"
#include "dheap.h"
#include "files.h"

#define WAD_MAX_FILES 64
#define WAD_ENTRY_DISK_SIZE 25

typedef struct WadEntryHeader {
    /* 0x00 */ char name[16];
    /* 0x10 */ s32 length;
    /* 0x14 */ s32 size;
    /* 0x18 */ u8 tag;
} WadEntryHeader;

typedef struct WADFILE {
    /* 0x000 */ u32 fileOffset;
    /* 0x004 */ s16 count;
    /* 0x006 */ s16 current;
    /* 0x008 */ s32 sizes[WAD_MAX_FILES];
    /* 0x108 */ s32 offsets[WAD_MAX_FILES];
} WADFILE;

void SwapEndian32(void *ptr);
void SwapEndian16(void *ptr);

WADFILE *OpenWadFile(u32 fileOffset);
WADFILE *OpenWadFileNvc(u32 fileOffset);
void CloseWadFileNvc(WADFILE *wad);
void CloseWadFile(WADFILE *wad);
void ReadNextWadFile(WADFILE *wad, void *dst);

#endif /* WIPEOUT64_WADFILE_H */
