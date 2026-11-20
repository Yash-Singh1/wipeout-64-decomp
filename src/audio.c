/*
 * audio.c — N64 libmus audio initialization, ROM streaming, and music control.
 *
 * Manages the Software Creations libmus driver (20 channels NTSC USA, 24
 * channels PAL Europe), PI DMA transfers for the pointer bank and 9 music
 * tracks, master SFX/music volumes, and pause/resume fading.
 */
#include "audio.h"

/* Header revision tags. Only #ident gives retail's packed, unaligned .rodata strings. */
#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

void InitAudio(void) {
    musConfig config;
    s32 ptrBankSize;

    osCreateMesgQueue(&g_audioDmaMsgQ, g_audioDmaMsgs, NUM_AUDIO_DMA_MSGS);
    config.control_flag = 0;
    g_audioHeap = DAlloc(g_heap, AUDIO_HEAP_SIZE, "audio.c", 119);

    ptrBankSize = _ptrBankRomEnd - _ptrBankRomStart;
    g_ptrBank = DAlloc(g_heap, ptrBankSize + 0x20, "audio.c", 125);
    AudioDmaRead(_ptrBankRomStart, g_ptrBank, ptrBankSize);
    LoadSongs();

#ifdef VERSION_EU
    config.channels = 24;
#else
    config.channels = 20;
#endif
    config.sched = &g_scheduler;
    config.thread_priority = 12;
    config.heap = g_audioHeap;
    config.heap_length = AUDIO_HEAP_SIZE;
    config.ptr = g_ptrBank;
    config.wbk = _waveBankRomStart;
    config.fxs = g_fxList;
    config.priority = g_fxPriority;
    config.syn_output_rate = 22050;
    config.syn_updates = 256;
    config.syn_rsp_cmds = 0x1000;
    config.syn_retraceCount = 1;
    config.syn_num_dma_bufs = 36;
    config.syn_dma_buf_size = 0x1000;
    MusInitialize(&config);
}

void AudioDmaRead(u8 *romAddr, u8 *dst, s32 length) {
    OSIoMesg ioMsg;
    OSMesg msg;
    s32 chunk;

    while (length != 0) {
        chunk = length;
        if (chunk > AUDIO_DMA_CHUNK) {
            chunk = AUDIO_DMA_CHUNK;
        }
        osInvalDCache(dst, chunk);
        osPiStartDma(&ioMsg, OS_MESG_PRI_NORMAL, OS_READ, (u32)romAddr, dst, chunk, &g_audioDmaMsgQ);
        osRecvMesg(&g_audioDmaMsgQ, &msg, OS_MESG_BLOCK);
        romAddr += chunk;
        length -= chunk;
        dst += chunk;
    }
}

void StartMusic(void) {
    s32 track;
    u8 *song;
    s32 volume;

    if (g_audioActive) {
        return;
    }

    do {
        track = newRand() % 8;
    } while (g_lastTrack == track);

    if (frontendOptions_g.musicTrack != RANDOM_TRACK) {
        track = frontendOptions_g.musicTrack;
    }
    song = GetSong(track);

    if (frontendOptions_g.musicVolume != 0) {
        volume = frontendOptions_g.musicVolume * 0x7FFF / 116 * g_trackVolume[track] / 100;
    } else {
        volume = 1;
    }
    SetMasterMusicVolume(volume);

    g_songHandle = MusStartSong(song);
    g_audioActive = TRUE;
    g_lastTrack = track;
    frontendOptions_g.currentTrack = track;
}

void StopAudio(void) {
    if (g_audioActive) {
        MusStop(MUSFLAG_EFFECTS | MUSFLAG_SONGS, 10);
        g_audioActive = FALSE;
    }
}

void SetMasterSfxVolume(s32 volume) {
    MusSetMasterVolume(MUSFLAG_EFFECTS, volume);
}

void SetMasterMusicVolume(s32 volume) {
    MusSetMasterVolume(MUSFLAG_SONGS, volume);
}

void ResumeAudio(void) {
    if (g_audioActive) {
        return;
    }

    if (frontendOptions_g.musicVolume != 0) {
        SetMasterMusicVolume(frontendOptions_g.musicVolume * 0x7FFF / 116);
    } else {
        SetMasterMusicVolume(1);
    }

    if (frontendOptions_g.sfxVolume != 0) {
        SetMasterSfxVolume(frontendOptions_g.sfxVolume * 0x7FFF / 116);
    } else {
        SetMasterSfxVolume(1);
    }

    g_audioActive = TRUE;
}

void PauseAudio(void) {
    if (g_audioActive) {
        SetMasterMusicVolume(frontendOptions_g.musicVolume * 0x7FFF / 11600);
        SetMasterSfxVolume(0);
        g_audioActive = FALSE;
    }
}

u8 *GetSong(s32 track) {
    if (track == TITLE_TRACK) {
        return g_songs[8];
    }
    return g_songs[track];
}

void LoadSongs(void) {
    s32 size0;
    s32 size1;
    s32 size2;
    s32 size3;
    s32 size4;
    s32 size5;
    s32 size6;
    s32 size7;
    s32 size8;

    size0 = _song0RomEnd - _song0RomStart;
    g_songData0 = DAlloc(g_heap, size0 + 0x20, "audio.c", 454);
    size1 = _song1RomEnd - _song1RomStart;
    g_songData1 = DAlloc(g_heap, size1 + 0x20, "audio.c", 455);
    size2 = _song2RomEnd - _song2RomStart;
    g_songData2 = DAlloc(g_heap, size2 + 0x20, "audio.c", 456);
    size3 = _song3RomEnd - _song3RomStart;
    g_songData3 = DAlloc(g_heap, size3 + 0x20, "audio.c", 457);
    size4 = _song4RomEnd - _song4RomStart;
    g_songData4 = DAlloc(g_heap, size4 + 0x20, "audio.c", 458);
    size5 = _song5RomEnd - _song5RomStart;
    g_songData5 = DAlloc(g_heap, size5 + 0x20, "audio.c", 459);
    size6 = _song6RomEnd - _song6RomStart;
    g_songData6 = DAlloc(g_heap, size6 + 0x20, "audio.c", 460);
    size7 = _song7RomEnd - _song7RomStart;
    g_songData7 = DAlloc(g_heap, size7 + 0x20, "audio.c", 461);
    size8 = _song8RomEnd - _song8RomStart;
    g_songData8 = DAlloc(g_heap, size8 + 0x20, "audio.c", 462);

    AudioDmaRead(_song0RomStart, g_songData0, size0);
    AudioDmaRead(_song1RomStart, g_songData1, size1);
    AudioDmaRead(_song2RomStart, g_songData2, size2);
    AudioDmaRead(_song3RomStart, g_songData3, size3);
    AudioDmaRead(_song4RomStart, g_songData4, size4);
    AudioDmaRead(_song5RomStart, g_songData5, size5);
    AudioDmaRead(_song6RomStart, g_songData6, size6);
    AudioDmaRead(_song7RomStart, g_songData7, size7);
    AudioDmaRead(_song8RomStart, g_songData8, size8);

    g_songs[0] = g_songData0;
    g_songs[1] = g_songData1;
    g_songs[2] = g_songData2;
    g_songs[3] = g_songData3;
    g_songs[4] = g_songData4;
    g_songs[5] = g_songData5;
    g_songs[6] = g_songData6;
    g_songs[7] = g_songData7;
    g_songs[8] = g_songData8;
}
