/*
 * files.c — Cartridge ROM file-table lookup and synchronous PI DMA loading.
 *
 * Resolves relative file offsets against _romFilesBase (USA 0xB03CDF10,
 * Europe 0xB03CD4D0) and streams asset files into the dynamic heap.
 */
#include "files.h"

void InitFileSystem(void) {
    s32 i;
    s32 count;
    s32 trackFile;
    u32 *romOff;

    count = 0;
    romOff = g_romFileOffsets;
    while (*romOff != (u32)_romFilesEnd) {
        romOff++;
        count++;
    }

    g_fileCount = count;
    g_fileTable = (RomFileEntry *)DAlloc(g_heap, (count + 1) * sizeof(RomFileEntry), "files.c", 230);

    romOff = g_romFileOffsets;
    for (i = 0; i < count; i++, romOff++) {
        g_fileTable[i].romAddr = romOff[0] + (u32)_romFilesBase;
        g_fileTable[i].length = romOff[1] - romOff[0];
    }

    g_fileTable[i].romAddr = 0;
    g_fileTable[i].length = 0;

    trackFile = FindFile((u32)_trackFilesStart);
    for (i = 0; i < 7; i++) {
        g_trackFileIds[i].f0 = trackFile++;
        g_trackFileIds[i].f1 = trackFile++;
        g_trackFileIds[i].f2 = trackFile++;
        g_trackFileIds[i].f3 = trackFile++;
        g_trackFileIds[i].f4 = trackFile++;
        g_trackFileIds[i].f5 = trackFile++;
    }
}

s32 FileLength(u32 fileOffset) {
    s32 idx;

    idx = FindFile(fileOffset);
    if (idx != -1) {
        return g_fileTable[idx].length;
    }
    return 0;
}

s32 LoadFile(u32 fileOffset, void *dst) {
    s32 idx;

    idx = FindFile(fileOffset);
    if (idx != -1) {
        DmaRead(g_fileTable[idx].romAddr, dst, g_fileTable[idx].length);
    }
    return g_fileTable[idx].length;
}

s32 FindFile(u32 fileOffset) {
    s32 i;
    u32 target;

    i = 0;
    if (g_fileTable[0].romAddr != 0) {
        target = fileOffset + (u32)_romFilesBase;
        while (g_fileTable[i].romAddr != 0) {
            if (g_fileTable[i].romAddr == target) {
                return i;
            }
            i++;
        }
    }
    return -1;
}

void LoadFileOffset(u32 fileOffset, void *dst, s32 offset, s32 length) {
    s32 idx;
    u32 dmaSrc;
    void *dmaDst;
    s32 dmaLen;
    s32 unaligned;

    idx = FindFile(fileOffset);
    if (idx != -1) {
        if (length + offset > g_fileTable[idx].length) {
            length = g_fileTable[idx].length - offset;
        }
        dmaLen = length;
        unaligned = 0;
        dmaSrc = 0;
        if (((offset + g_fileTable[idx].romAddr) & 7) != 0) {
            unaligned = 1;
        } else {
            dmaSrc = offset + g_fileTable[idx].romAddr;
        }
        if (((u32)dst & 7) != 0) {
            unaligned = 1;
        }
        if (unaligned) {
            dmaDst = DAlloc(g_heap, dmaLen + 16, "files.c", 545);
            if (dmaSrc == 0) {
                dmaSrc = (offset + g_fileTable[idx].romAddr - 7) & ~7;
                dmaLen += (offset + g_fileTable[idx].romAddr) - dmaSrc;
            }
        } else {
            dmaDst = dst;
        }
        DmaRead(dmaSrc, dmaDst, dmaLen);
        if (unaligned) {
            bcopy((u8 *)dmaDst + ((offset + g_fileTable[idx].romAddr) - dmaSrc), dst, length);
            DFree(g_heap, dmaDst);
        }
    }
}
