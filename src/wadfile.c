#include "wadfile.h"

WADFILE *OpenWadFile(u32 fileOffset) {
    WADFILE *wad;
    s32 i;
    s32 offset;
    s32 dataOffset;
    WadEntryHeader entry;

    wad = (WADFILE *)DAlloc(g_heap, sizeof(WADFILE), "Wadfile.c", 606);
    wad->fileOffset = fileOffset;
    if (FindFile(fileOffset) == -1) {
        return NULL;
    }

    LoadFileOffset(fileOffset, &wad->count, 0, sizeof(wad->count));
    SwapEndian16(&wad->count);
    offset = 2;
    dataOffset = 0;

    for (i = 0; i < wad->count; i++) {
        LoadFileOffset(fileOffset, &entry, offset, WAD_ENTRY_DISK_SIZE);
        offset += WAD_ENTRY_DISK_SIZE;
        SwapEndian32(&entry.length);
        SwapEndian32(&entry.size);
        wad->sizes[i] = entry.size;
        wad->offsets[i] = dataOffset;
        dataOffset += entry.size;
    }

    for (i = 0; i < wad->count; i++) {
        wad->offsets[i] += offset;
    }

    wad->current = 0;
    return wad;
}

WADFILE *OpenWadFileNvc(u32 fileOffset) {
    WADFILE *wad;
    s32 i;
    s32 offset;
    s32 dataOffset;
    WadEntryHeader entry;

    wad = (WADFILE *)DAlloc(g_heap, sizeof(WADFILE), "Wadfile.c", 766);
    wad->fileOffset = fileOffset;
    if (FindFile(fileOffset) == -1) {
        return NULL;
    }

    LoadFileOffset(fileOffset, &wad->count, 0, sizeof(wad->count));
    SwapEndian16(&wad->count);
    offset = 2;
    dataOffset = 0;

    for (i = 0; i < wad->count; i++) {
        LoadFileOffset(fileOffset, &entry, offset, WAD_ENTRY_DISK_SIZE);
        offset += WAD_ENTRY_DISK_SIZE;
        SwapEndian32(&entry.length);
        SwapEndian32(&entry.size);
        wad->sizes[i] = entry.size;
        wad->offsets[i] = dataOffset;
        dataOffset += entry.size;
    }

    for (i = 0; i < wad->count; i++) {
        wad->offsets[i] += offset;
    }

    wad->current = 0;
    return wad;
}

void CloseWadFileNvc(WADFILE *wad) {
    DFree(g_heap, wad);
}

void CloseWadFile(WADFILE *wad) {
    DFree(g_heap, wad);
}

void ReadNextWadFile(WADFILE *wad, void *dst) {
    LoadFileOffset(wad->fileOffset, dst, wad->offsets[wad->current], wad->sizes[wad->current]);
    wad->current++;
}
