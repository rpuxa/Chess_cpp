//
// Created by Пользователь on 16.07.2017.
//

#ifndef CHESS_MASK_H
#define CHESS_MASK_H

#include "def.h"
namespace mask {
    const double log2 = log(2);
    extern ull_vector castle_white;
    extern ull_vector castle_black;
    extern ull_matrix pawn_attack;
    extern ull_matrix pawn_move;
    extern ull_vector knight;
    extern ull_vector king;
    extern ull_matrix bishop_R;
    extern ull_matrix bishop_L;
    extern ull_matrix rook_G;
    extern ull_matrix rook_V;
    extern ull_vector cell_default;
    extern ull_vector cell_rotated_90;
    extern ull_vector cell_rotated_45_left;
    extern ull_vector cell_rotated_45_right;

}

bool B(int x, int y);

ull rotate_90(ull mask);

ull rotate_R(ull mask);

ull rotate_L(ull mask);

void calculating();


#endif //CHESS_MASK_H
