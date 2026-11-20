#ifndef WIPEOUT64_FILES_H
#define WIPEOUT64_FILES_H

#include "common.h"
#include "dheap.h"

extern u8 _romFilesBase[];
extern u8 _romFilesEnd[];
extern u8 _trackFilesStart[];

typedef struct {
    u32 romAddr;
    s32 length;
} RomFileEntry;

typedef struct {
    s32 f0;
    s32 f1;
    s32 f2;
    s32 f3;
    s32 f4;
    s32 f5;
} TrackFileGroup;

extern s32 g_fileCount;
extern u32 g_romFileOffsets[];
extern RomFileEntry *g_fileTable;
extern TrackFileGroup g_trackFileIds[7];

void bcopy(const void *src, void *dst, int length);
void DmaRead(u32 romAddr, void *dst, s32 length);

void InitFileSystem(void);
s32 FileLength(u32 fileOffset);
s32 LoadFile(u32 fileOffset, void *dst);
s32 FindFile(u32 fileOffset);
void LoadFileOffset(u32 fileOffset, void *dst, s32 offset, s32 length);

#endif /* WIPEOUT64_FILES_H */
