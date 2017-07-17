

#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H
#include "def.h"

namespace figures{
    const int  pawn = 0;
    const int  rook = 1;
    const int  knight = 2;
    const int  bishop = 3;
    const int  queen = 4;
    const int  king = 5;
}

class Bitboard {
public:
    ull_vector white;
    ull_vector black;
    ull pass;
    bool castle;
    ull_vector all;
    bool lastMove;

public:
    Bitboard(ull_vector white,ull_vector black,ull pass,bool castle,ull_vector all,bool lastMove);


    void add(uint_vector & moves,ui pos, ull mask,ui figureMove, bool promotion);

    void add(uint_vector & moves,ui pos, ull mask,ui figureMove);

    bool attackCell(bool isTurnWhite,int cell);

    int cost(int to);

    bool isCheckTo(bool White);

    bool canCastle(int type);

    uint_vector getMoves(bool isTurnWhite, bool capture);

    uint_vector getMoves(bool isTurnWhite);
};


Bitboard make_bitboard_empty();

Bitboard make_bitboard_start();

Bitboard make_bitboard_from(Bitboard bitboard);

Bitboard make_bitboard_nullMove_from(Bitboard bitboard);

#endif //CHESS_BITBOARD_H
