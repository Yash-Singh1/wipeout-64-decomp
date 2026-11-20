.section .rodata, "a"
.balign 4
dlabel D_80003500
    /* 4100 80003500 */ .asciz "MUSIC TRACK"
    /* 4D5553494320545241434B00 */
.align 2
enddlabel D_80003500

.balign 4
dlabel D_8000350C
    /* 410C 8000350C */ .asciz "BANG ON"
    /* 42414E47204F4E00 */
.align 2
enddlabel D_8000350C

.balign 4
dlabel D_80003514
    /* 4114 80003514 */ .asciz "[%s]"
    /* 5B25735D00000000 */
.align 2
enddlabel D_80003514

.balign 4
dlabel D_8000351C
    /* 411C 8000351C */ .asciz "MUSIC VOLUME"
    /* 4D5553494320564F4C554D4500000000 */
.align 2
enddlabel D_8000351C

.balign 4
dlabel D_8000352C
    /* 412C 8000352C */ .asciz "SFX VOLUME"
    /* 53465820564F4C554D450000 */
.align 2
enddlabel D_8000352C

