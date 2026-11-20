#ifndef WIPEOUT64_AUDIO_H
#define WIPEOUT64_AUDIO_H

#include <ultra64.h>
#include <PR/sched.h>
#include "common.h"
#include "dheap.h"


/* Software Creations libmus (older interface: no fifo_length, sound effect lists in the config). */
#define MUSFLAG_EFFECTS 1
#define MUSFLAG_SONGS   2

/* TEMPORARY: sfx.h also defines musHandle; both belong in one libmus header. */
#ifndef WIPEOUT64_SFX_H
typedef u32 musHandle;
#endif

typedef struct {
    /* 0x00 */ unsigned long control_flag;
    /* 0x04 */ int channels;
    /* 0x08 */ OSSched *sched;
    /* 0x0C */ int thread_priority;
    /* 0x10 */ unsigned char *heap;
    /* 0x14 */ int heap_length;
    /* 0x18 */ unsigned char *ptr;
    /* 0x1C */ unsigned char *wbk;
    /* 0x20 */ unsigned char **fxs;
    /* 0x24 */ int *priority;
    /* 0x28 */ int syn_updates;
    /* 0x2C */ int syn_output_rate;
    /* 0x30 */ int syn_rsp_cmds;
    /* 0x34 */ int syn_retraceCount;
    /* 0x38 */ int syn_num_dma_bufs;
    /* 0x3C */ int syn_dma_buf_size;
} musConfig;

int MusInitialize(musConfig *config);
int MusSetMasterVolume(unsigned long flags, int volume);
musHandle MusStartSong(void *addr);
int MusStop(unsigned long flags, int speed);

#define AUDIO_HEAP_SIZE 0x3F7A0
#define AUDIO_DMA_CHUNK 0x4000
#define NUM_AUDIO_DMA_MSGS 32

#define NUM_SONGS    9
#define RANDOM_TRACK 8
#define TITLE_TRACK  9

/*
 * TEMPORARY: radscn.h declares FrontendOptions only up to numPlayers. audio.c also needs the
 * track and volume fields; fold them into radscn.h and drop this block. Files that include
 * radscn.h before this header keep its shorter declaration.
 */
#ifndef WIPEOUT64_RADSCN_H
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[7];
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[0x5B];
    /* 0x6C */ s32 musicTrack;
    /* 0x70 */ s32 currentTrack;
    /* 0x74 */ s32 musicVolume;
    /* 0x78 */ s32 sfxVolume;
} FrontendOptions;

extern FrontendOptions frontendOptions_g;
#endif

/*
 * Temporary names until these are added to config/{us,eu}/symbol_addrs.txt. Each ROM segment
 * end shares its address with the next segment's start but is a separate symbol (the compiler
 * must not reuse one for the other), so the ends use L<addr> names, which check.py also resolves.
 */
#ifdef VERSION_EU
#define g_audioActive       D_800895C0
#define g_trackVolume       D_800895C4
#define g_lastTrack         D_800895EC
#define g_fxList            D_80096660
#define g_fxPriority        D_80096780
#define g_songs             D_800A5CD0
#define g_songHandle        D_800A5CF4
#define g_audioDmaMsgQ      D_800A5CF8
#define g_ptrBank           D_800A5D10
#define g_songData0         D_800A5D14
#define g_songData1         D_800A5D18
#define g_songData2         D_800A5D1C
#define g_songData3         D_800A5D20
#define g_songData4         D_800A5D24
#define g_songData5         D_800A5D28
#define g_songData6         D_800A5D2C
#define g_songData7         D_800A5D30
#define g_songData8         D_800A5D34
#define g_audioDmaMsgs      D_800A5D38
#define g_audioHeap         D_800A5DB8
#define _waveBankRomStart   D_B0098B10
#define _ptrBankRomStart    D_B03AD060
#define _ptrBankRomEnd      LB03B4380
#define _song0RomStart      D_B03B4380
#define _song0RomEnd        LB03B8C20
#define _song1RomStart      D_B03B8C20
#define _song1RomEnd        LB03BB290
#define _song2RomStart      D_B03BB290
#define _song2RomEnd        LB03BF7E0
#define _song3RomStart      D_B03BF7E0
#define _song3RomEnd        LB03C12B0
#define _song4RomStart      D_B03C12B0
#define _song4RomEnd        LB03C7940
#define _song5RomStart      D_B03C7940
#define _song5RomEnd        LB03C85C0
#define _song6RomStart      D_B03C85C0
#define _song6RomEnd        LB03C8B40
#define _song7RomStart      D_B03C8B40
#define _song7RomEnd        LB03C8FF0
#define _song8RomStart      D_B03C8FF0
#define _song8RomEnd        LB03CD4D0
#define newRand             func_80023FCC
#define MusInitialize       func_80073168
#define MusSetMasterVolume  func_8007348C
#define MusStartSong        func_800734B8
#define MusStop             func_800737C0
#define AudioDmaRead        func_8000FBD8
#define StartMusic          func_8000FC8C
#define SetMasterSfxVolume        func_8000FDF0
#define SetMasterMusicVolume      func_8000FE14
#define ResumeAudio         func_8000FE38
#define PauseAudio          func_8000FEF0
#define GetSong             func_8000FF58
#define LoadSongs           func_8000FF8C
#else
#define g_audioActive       D_80089F90
#define g_trackVolume       D_80089F94
#define g_lastTrack         D_80089FBC
#define g_fxList            D_800970B0
#define g_fxPriority        D_800971D0
#define g_songs             D_800986F0
#define g_songHandle        D_80098714
#define g_audioDmaMsgQ      D_80098718
#define g_ptrBank           D_80098730
#define g_songData0         D_80098734
#define g_songData1         D_80098738
#define g_songData2         D_8009873C
#define g_songData3         D_80098740
#define g_songData4         D_80098744
#define g_songData5         D_80098748
#define g_songData6         D_8009874C
#define g_songData7         D_80098750
#define g_songData8         D_80098754
#define g_audioDmaMsgs      D_80098758
#define g_audioHeap         D_800987D8
#define _waveBankRomStart   D_B0099550
#define _ptrBankRomStart    D_B03ADAA0
#define _ptrBankRomEnd      LB03B4DC0
#define _song0RomStart      D_B03B4DC0
#define _song0RomEnd        LB03B9660
#define _song1RomStart      D_B03B9660
#define _song1RomEnd        LB03BBCD0
#define _song2RomStart      D_B03BBCD0
#define _song2RomEnd        LB03C0220
#define _song3RomStart      D_B03C0220
#define _song3RomEnd        LB03C1CF0
#define _song4RomStart      D_B03C1CF0
#define _song4RomEnd        LB03C8380
#define _song5RomStart      D_B03C8380
#define _song5RomEnd        LB03C9000
#define _song6RomStart      D_B03C9000
#define _song6RomEnd        LB03C9580
#define _song7RomStart      D_B03C9580
#define _song7RomEnd        LB03C9A30
#define _song8RomStart      D_B03C9A30
#define _song8RomEnd        LB03CDF10
#define newRand             func_8001CFAC
#define MusInitialize       func_800733C8
#define MusSetMasterVolume  func_800736EC
#define MusStartSong        func_80073718
#define MusStop             func_80073A20
#define AudioDmaRead        func_80008DA8
#define StartMusic          func_80008E5C
#define SetMasterSfxVolume        func_80008FC0
#define SetMasterMusicVolume      func_80008FE4
#define ResumeAudio         func_80009008
#define PauseAudio          func_800090C0
#define GetSong             func_80009128
#define LoadSongs           func_8000915C
#endif

extern s32 g_audioActive;
extern s32 g_trackVolume[10];
extern s32 g_lastTrack;
extern unsigned char *g_fxList[];
extern int g_fxPriority[];
extern u8 *g_songs[NUM_SONGS];
extern musHandle g_songHandle;
extern OSMesgQueue g_audioDmaMsgQ;
extern u8 *g_ptrBank;
extern u8 *g_songData0;
extern u8 *g_songData1;
extern u8 *g_songData2;
extern u8 *g_songData3;
extern u8 *g_songData4;
extern u8 *g_songData5;
extern u8 *g_songData6;
extern u8 *g_songData7;
extern u8 *g_songData8;
extern OSMesg g_audioDmaMsgs[NUM_AUDIO_DMA_MSGS];
extern u8 *g_audioHeap;
extern OSSched g_scheduler;

/* ROM segments of the sound banks and songs, which precede the file table. */
extern u8 _waveBankRomStart[];
extern u8 _ptrBankRomStart[];
extern u8 _ptrBankRomEnd[];
extern u8 _song0RomStart[];
extern u8 _song0RomEnd[];
extern u8 _song1RomStart[];
extern u8 _song1RomEnd[];
extern u8 _song2RomStart[];
extern u8 _song2RomEnd[];
extern u8 _song3RomStart[];
extern u8 _song3RomEnd[];
extern u8 _song4RomStart[];
extern u8 _song4RomEnd[];
extern u8 _song5RomStart[];
extern u8 _song5RomEnd[];
extern u8 _song6RomStart[];
extern u8 _song6RomEnd[];
extern u8 _song7RomStart[];
extern u8 _song7RomEnd[];
extern u8 _song8RomStart[];
extern u8 _song8RomEnd[];

s32 newRand(void);

void InitAudio(void);
void AudioDmaRead(u8 *romAddr, u8 *dst, s32 length);
void StartMusic(void);
void StopAudio(void);
void SetMasterSfxVolume(s32 volume);
void SetMasterMusicVolume(s32 volume);
void ResumeAudio(void);
void PauseAudio(void);
u8 *GetSong(s32 track);
void LoadSongs(void);

#endif /* WIPEOUT64_AUDIO_H */
