#include "Bitboard.h"
#include "main.h"
#include "mask.h"

int main() {
    calculating();
   Bitboard bitboard = make_bitboard_start();
    cout << "1";
    for (int i = 0; i < 1000*1000; ++i)
    bitboard.getMoves(true);
    return 0;
}

ull setBit(ull mask, int to){
    return mask | (1ULL << to);
}

bool getBit(ull mask, int to){
    return ((mask >> to) & 1) != 0;
}

ull zeroBit(ull mask, int to){
    return mask & ~(1ULL << to);
}

ull swapBit(ull mask, int to){
    return mask ^ (1ULL << to);
}