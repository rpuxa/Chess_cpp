
#include "Bitboard.h"
#include "mask.h"
#include "main.h"
#include "eval.h"
#include "def.h"

using namespace mask;
using namespace eval;

const vector<ull> WHITE_BASE = {0b1111111100000000LL,0b10000001LL,0b1000010LL,0b100100LL,0b10000LL,0b1000LL};

const vector<ull> BLACK_BASE = {0b1111111100000000LL << 40,0b10000001ULL << 56,0b1000010LL << 56,0b100100LL << 56,0b10000LL << 56,0b1000LL << 56};

const ui fig[4] = {figures::queen,figures::rook,figures::knight,figures::bishop};

ull_vector all_init(ull_vector & white, ull_vector & black) {
    ull ALL = 0;
    ull white2 = 0, black2 = 0;
    for (int i = 0; i < white.size(); ++i) {
        white2 |= white[i];
        black2 |= black[i];
    }
    ALL = white2 | black2;

    ull all_rotated_90 = 0;

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            if (((ALL >> (8 * y + x)) & 1) != 0)
                all_rotated_90 |= 1ULL << 8 * (7 - x) + y;

    ull all_rotated_45_left = 0;
    int n = 0;
    for (int i = 0; i < 8; i++) {
        int m = 0;
        for (int j = 7 - i; j <= 7; j++) {
            if (((ALL >> j + 8 * m) & 1) != 0)
                all_rotated_45_left |= 1ULL << n;
            m++;
            n++;
        }
    }
    for (int i = 8; i < 16; i++) {
        int m = 0;
        for (int j = 7 - i; j <= 7; j++) {
            if (j + 8 * m > -1 && j + 8 * m < 64 && (j + 8 * m) / 8 > (j + 8 * m) % 8) {
                if (((ALL >> j + 8 * m) & 1) != 0)
                    all_rotated_45_left |= 1ULL << n;
                n++;
            }
            m++;
        }
    }

    ull all_rotated_45_right = 0;

    n = 0;
    for (int i = 0; i < 8; i++) {
        int m = 8 * i;
        for (int j = 0; j <= i; j++) {
            if (((ALL >> j + m) & 1) != 0)
                all_rotated_45_right |= 1ULL << n;
            m -= 8;
            n++;
        }
    }
    for (int i = 8; i < 16; i++) {
        int m = 8 * i;
        for (int j = 0; j <= i; j++) {
            if (j + m > -1 && j + m < 64 && (j + m) / 8 + (j + m) % 8 > 7) {
                if (((ALL >> j + m) & 1) != 0)
                    all_rotated_45_right |= 1ULL << n;
                n++;
                if (j + m == 63)
                    goto stop;
            }
            m -= 8;
        }
    }
    stop:
    ull_vector tmp = {ALL,all_rotated_90,all_rotated_45_left,all_rotated_45_right};
    return tmp;
}

Bitboard make_bitboard_empty(){
    vector<ull> white(6,0);
    vector<ull> black(6,0);
    ull pass = 0;
    bool castle = 0b1111;
    vector<ull> all(4,0);
    bool lastMove = 0;
    return Bitboard(white,black,pass,castle,all,lastMove);
}

Bitboard make_bitboard_start(){
    vector<ull> white = WHITE_BASE;
    vector<ull> black = BLACK_BASE;
    ull pass = 0;
    bool castle = 0b1111;
    vector<ull> all(all_init(white,black));
    bool lastMove = 0;
    return Bitboard(white,black,pass,castle,all,lastMove);
}

Bitboard make_bitboard_from(Bitboard bitboard){
    return Bitboard(bitboard.white,bitboard.black,bitboard.pass,bitboard.castle,bitboard.all,bitboard.lastMove);
}

Bitboard make_bitboard_nullMove_from(Bitboard bitboard){
    return Bitboard(bitboard.white,bitboard.black,0ULL,bitboard.castle,bitboard.all,bitboard.lastMove);
}

Bitboard::Bitboard(ull_vector & white,ull_vector & black,ull pass,bool castle,ull_vector & all,bool lastMove)
            :white(white), black(black), pass(pass), castle(castle), all(all), lastMove(lastMove) {}

    void Bitboard::add(uint_vector & moves,ui pos, ull mask,ui figureMove, bool promotion) {
        if (mask != 0)
            for (int i = 0; i < 64; i++)
                if (getBit(mask,i))
                    if (!promotion) {
                        moves.push_back(((((pos << 6) + i) << 4) + figureMove) << 3);
                    } else {
                        for (ui figure :fig)
                            moves.push_back((((((pos << 6) + i) << 4) + figureMove) << 3) + figure);
                    }
    }

    void Bitboard::add(uint_vector & moves,ui pos, ull mask,ui figureMove) {
        add(moves,pos,mask,figureMove,false);
    }

    bool Bitboard::attackCell(bool isTurnWhite,int cell){
        vector<ull> *enemyFigures = (!isTurnWhite) ? &white : &black;
        if ((mask::knight[cell] & enemyFigures ->operator[](figures::knight)) != 0)
            return true;
        if ((mask::king[cell] & enemyFigures ->operator[](figures::king)) != 0)
            return true;
        if ((((isTurnWhite) ? (black[figures::pawn] & mask::pawn_attack[1][cell]) : (white[figures::pawn] & mask::pawn_attack[0][cell]))) != 0)
            return true;
        ull attack;
        int nd = cell / 8;
        ull mask = (all[0] >> 8 * nd) & 255;
        attack = mask::rook_G[cell][(int) mask];
        nd = 7 - cell % 8;
        mask = (all[1] >> 8 * nd) & 255;
        attack |= mask::rook_V[cell][(int) mask];

        if ((attack & (enemyFigures ->operator[](figures::rook) | enemyFigures ->operator[](figures::queen))) != 0)
            return true;

        nd = cell / 8 - cell % 8 + 7;
        mask = all[2] >> ((nd >= 9) ? ((21 - nd) * (nd - 8) / 2 + 36) : (nd * (nd + 1) / 2)) & 255;
        attack = mask::bishop_L[cell][(int) mask];
        nd = cell / 8 + cell % 8;
        mask = all[3] >> ((nd >= 9) ? ((21 - nd) * (nd - 8) / 2 + 36) : (nd * (nd + 1) / 2)) & 255;
        attack |= mask::bishop_R[cell][(int) mask];

        return (attack & (enemyFigures ->operator[](figures::bishop) | enemyFigures ->operator[](figures::queen))) != 0;

    }

    int Bitboard::cost(int to){
        if (to == 0){
            return val_P;
        } else if (to == 1){
            return val_R;
        } else if (to == 2){
            return val_N;
        } else if (to == 3){
            return val_B;
        } else if (to == 4){
            return val_Q;
        } else if (to == 5){
            return val_K;
        } else if (to == 6){
            return -val_P;
        } else if (to == 7){
            return -val_R;
        } else if (to == 8) {
            return -val_N;
        } else if (to == 9){
            return -val_B;
        } else if (to == 10){
            return -val_Q;
        } else if (to == 11){
            return -val_K;
        }
        return 0;
    }

    bool Bitboard::isCheckTo(bool White){
        return attackCell(White, (int)round(log(((White) ? white[figures::king] : black[figures::king])) / mask::log2));
    }

    bool Bitboard::canCastle(int type){
        vector<int> cells;
        if (type == 0)
            cells = {1,2,3};
        else if (type == 1)
            cells = {3,4,5};
        else if (type == 2)
            cells = {57,58,59};
        else if (type == 3)
            cells = {59,60,61};
        bool isTurnWhite = type <=1;
        for (int cell : cells) {
            if (attackCell(isTurnWhite,cell))
                return false;
        }
        return true;
    }

    uint_vector Bitboard::getMoves(bool isTurnWhite, bool capture){
        uint_vector moves;
        ui figure = ((isTurnWhite) ? 0:6);
        vector<ull> *ourFigures = (isTurnWhite) ? &white : &black;
        vector<ull> *enemyFigures = (!isTurnWhite) ? &white : &black;
        ull our = ourFigures -> operator[](figures::pawn) | ourFigures -> operator[](figures::rook) | ourFigures -> operator[](figures::knight) | ourFigures -> operator[](figures::bishop) | ourFigures -> operator[](figures::queen) | ourFigures -> operator[](figures::king);
        ull enemy = enemyFigures -> operator[](figures::pawn) | enemyFigures -> operator[](figures::rook) | enemyFigures -> operator[](figures::knight) | enemyFigures -> operator[](figures::bishop) | enemyFigures -> operator[](figures::queen) | enemyFigures -> operator[](figures::king);
        ull capture_mask = ((capture) ? enemy : (ull) ~0L);
        for (ui i = 0; i < 64; ++i) {
            bool queen = getBit(ourFigures -> operator[](figures::queen),i);
            if (getBit(ourFigures -> operator[](figures::pawn),i)){
                bool promotion = i / 8 == ((isTurnWhite) ? 6 : 1);
                if (!getBit(all[0],i + 8 * ((isTurnWhite) ? 1 : -1)))
                    add(moves,i,mask::pawn_move[isTurnWhite][i] & ~all[0] & capture_mask,figures::pawn + figure,promotion);
                add(moves,i,mask::pawn_attack[isTurnWhite][i] & (enemy | pass),figures::pawn + figure,promotion);
            } else if (getBit(ourFigures -> operator[](figures::knight),i)){
                add(moves,i, mask::knight[i] & ~our & capture_mask,figures::knight + figure);
            } else if (getBit(ourFigures -> operator[](figures::king),i)){
                add(moves,i, mask::king[i] & ~our & capture_mask,figures::king + figure);
                if (!capture)
                if (isTurnWhite) {
                    if (i==3)
                        if ((all[0] & mask::castle_white[0]) == 0 && getBit(white[figures::rook],0) && getBit((ull)castle,3) && canCastle(0))
                            moves.push_back(((193<<4) + 6) << 3);
                        else if ((all[0] & mask::castle_white[1]) == 0 && getBit(white[figures::rook],7) && getBit((ull)castle,2) && canCastle(1))
                            moves.push_back(((197<<4) + 6) << 3);
                } else {
                    if (i==59)
                        if ((all[0] & mask::castle_black[0]) == 0 && getBit(white[figures::rook],56) && getBit((ull)castle,1) && canCastle(2))
                            moves.push_back(((3833<<4) + 12) << 3);
                        else if ((all[0] & mask::castle_black[1]) == 0 && getBit(white[figures::rook],63) && getBit((ull)castle,0) && canCastle(3))
                            moves.push_back((((3837<<4) + 12) << 3));
                }
            } else if (getBit(ourFigures -> operator[](figures::rook),i) || queen){
                int nd = i / 8;
                ull mask = (all[0] >> 8 * nd) & 255;
                add(moves,i, mask::rook_G[i][(int)mask] & ~our & capture_mask,figures::rook + figure);
                nd = 7 - i % 8;
                mask = (all[1] >> 8 * nd) & 255;
                add(moves,i, mask::rook_V[i][(int)mask] & ~our & capture_mask,figures::rook + figure);
            } if (getBit(ourFigures -> operator[](figures::bishop),i) || queen) {
                int nd = i / 8 - i % 8 + 7;
                ull mask = all[2] >> ((nd >= 9) ? ((21 - nd) * (nd - 8) / 2 + 36) : (nd * (nd + 1) / 2)) & 255;
                add(moves,i, mask::bishop_L[i][(int)mask] & ~our & capture_mask,figures::bishop + figure);
                nd = i / 8 + i % 8;
                mask = all[3] >> ((nd >= 9) ? ((21 - nd) * (nd - 8) / 2 + 36) : (nd * (nd + 1) / 2)) & 255;
                add(moves,i, mask::bishop_R[i][(int)mask] & ~our & capture_mask,figures::bishop + figure);
            }
        }
        return moves;
    }

    uint_vector Bitboard::getMoves(bool isTurnWhite){
        return getMoves(isTurnWhite,false);
    }

