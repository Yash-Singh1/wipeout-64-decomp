#ident "$Revision: 1.17 $"

#include "common.h"

#ifdef VERSION_EU
#define Unk50740_ReturnArg2 func_800518D0
#else
#define Unk50740_ReturnArg2 func_8004FB40
#endif

s32 Unk50740_ReturnArg2(s32 a0, s32 a1, s32 a2) {
    return a2;
}
