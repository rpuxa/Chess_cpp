#ifndef CHESS_MAIN_H
#define CHESS_MAIN_H

#include "def.h"

typedef unsigned long long ull;

ull setBit(ull mask, int to);

bool getBit(ull mask, int to);

ull zeroBit(ull mask, int to);

ull swapBit(ull mask, int to);


#endif