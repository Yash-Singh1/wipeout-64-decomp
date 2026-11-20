/*
 * decomp.c — LZ/RNC bitstream decompressor for compressed texture archives.
 *
 * Expands compressed TIM texture sequence blobs loaded from cartridge ROM or
 * track WAD archives before palette conversion and N64 texture setup.
 */
#include "common.h"

void ExpandData(u8 *input, u8 *output) {
    s16 i, position, length, match;
    u8 ch, mask;
    s16 rack, literal;
    u32 bit, bits;
    u8 window[8192];

    rack = 0;
    mask = 128;
    position = 1;
    for (;;) {
        if (mask == 128) {
            rack = *input++;
        }
        literal = rack & mask;
        mask >>= 1;
        if (!mask) {
            mask = 128;
        }
        if (literal) {
            bit = 1UL << (8 - 1);
            bits = 0;
            while (bit) {
                if (mask == 128) {
                    rack = *input++;
                }
                if (rack & mask) {
                    bits |= bit;
                }
                bit >>= 1;
                mask >>= 1;
                if (!mask) {
                    mask = 128;
                }
            }
            ch = bits;
            *output++ = ch;
            window[position] = ch;
            position = (position + 1) & 8191;
        } else {
            bit = 1UL << (13 - 1);
            bits = 0;
            while (bit) {
                if (mask == 128) {
                    rack = *input++;
                }
                if (rack & mask) {
                    bits |= bit;
                }
                bit >>= 1;
                mask >>= 1;
                if (!mask) {
                    mask = 128;
                }
            }
            match = bits;
            if (!match) {
                break;
            }
            bit = 1UL << (4 - 1);
            bits = 0;
            while (bit) {
                if (mask == 128) {
                    rack = *input++;
                }
                if (rack & mask) {
                    bits |= bit;
                }
                bit >>= 1;
                mask >>= 1;
                if (!mask) {
                    mask = 128;
                }
            }
            length = bits;
            length += 2;
            for (i = 0; i <= length; i++) {
                ch = window[(match + i) & 8191];
                *output++ = ch;
                window[position] = ch;
                position = (position + 1) & 8191;
            }
        }
    }
}
