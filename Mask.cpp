#include "Mask.h"

namespace mask {
    ull_vector castle_white = {0b110LL,0b1110000LL};
    ull_vector castle_black = {0b110LL << 56,0b1110000LL << 56};
    ull_matrix pawn_attack(2, ull_vector(64, 0));
    ull_matrix pawn_move(2, ull_vector(64, 0));
    ull_vector knight(64);
    ull_vector king(64);
    ull_matrix bishop_R(64, ull_vector(256, 0));
    ull_matrix bishop_L(64, ull_vector(256, 0));
    ull_matrix rook_G(64, ull_vector(256, 0));
    ull_matrix rook_V(64, ull_vector(256, 0));
    ull_vector cell_default(64);
    ull_vector cell_rotated_90(64);
    ull_vector cell_rotated_45_left(64);
    ull_vector cell_rotated_45_right(64);

}

using namespace mask;

bool B(int x, int y) {
    return x >= 0 && x <= 7 && y >= 0 && y <= 7;
}

ull rotate_90(ull mask){
    ull rotate = 0;
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            if (((mask >> (8 * (7 - x) + y)) & 1) != 0) {
                rotate |= 1ULL << (8*y+x);
            }
    return rotate;
}

ull rotate_R(ull mask){
    ull rotated = 0;
    int n = 0;
    for (int i = 0; i < 8; i++) {
        int m = 8*i;
        for (int j = 0; j <= i; j++){
            if (((mask >> n) & 1) != 0)
                rotated |= 1ULL << j+m;
            m-=8;
            n++;
        }
    }
    for (int i = 8; i < 16; i++) {
    int m = 8*i;
    for (int j = 0; j <= i; j++){
        if (j+m>-1 && j+m<64 && (j+m)/8+(j+m)%8>7) {
            if (((mask >> n) & 1) != 0)
                rotated |= 1ULL << j+m;
            n++;
            if (j+m == 63)
                goto stop;
        }
        m-=8;
    }
}
    stop:
    return rotated;
}

ull rotate_L(ull mask){
    ull rotated = 0;
    int n = 0;
    for (int i = 0; i < 8; i++) {
        int m = 0;
        for (int j = 7 - i; j <= 7; j++){
            if (((mask >> n) & 1) != 0)
                rotated |= 1ULL << j+8*m;
            m++;
            n++;
        }
    }
    for (int i = 8; i < 16; i++) {
        int m = 0;
        for (int j = 7 - i; j <= 7; j++){
            if (j+8*m>-1 && j+8*m<64 && (j+8*m)/8>(j+8*m)%8) {
                if (((mask >> n) & 1) != 0)
                    rotated |= 1ULL << j+8*m;
                n++;
            }
            m++;
        }
    }
    return rotated;
}

void calculating() {
    int i = 0;
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            //cell
            cell_default[8 * y + x] |= 1ULL << (8 * y + x);
            //knight
            for (int y2 = -2; y2 <= 2; y2 += 4)
                for (int x2 = -1; x2 <= 1; x2 += 2)
                    if (B(y + y2, x + x2))
                        knight[i] |= 1ULL << (8 * (y + y2) + x + x2);
            for (int y2 = -1; y2 <= 1; y2 += 2)
                for (int x2 = -2; x2 <= 2; x2 += 4)
                    if (B(y + y2, x + x2))
                        knight[i] |= 1ULL << (8 * (y + y2) + x + x2);
            //White_pawn_at
            if (B(x + 1, y + 1))
                pawn_attack[1][i] |= 1ULL << (8 * (y + 1) + x + 1);
            if (B(x - 1, y + 1))
                pawn_attack[1][i] |= 1ULL << (8 * (y + 1) + x - 1);
            if (B(x, y + 1))
                pawn_move[1][i] |= 1ULL << (8 * (y + 1) + x);
            if (i >= 8 && i <= 15)
                pawn_move[1][i] |= 1ULL << (i + 8 * 2);
            //black pawn
            if (B(x + 1, y - 1))
                pawn_attack[0][i] |= 1ULL << (8 * (y - 1) + x + 1);
            if (B(x - 1, y - 1))
                pawn_attack[0][i] |= 1ULL << (8 * (y - 1) + x - 1);
            if (B(x, y - 1))
                pawn_move[0][i] |= 1ULL << (8 * (y - 1) + x);
            if (i >= 48 && i <= 55)
                pawn_move[0][i] |= 1ULL << (i - 8 * 2);

            //king
            int direction[8][2] = {{-1, 0},
                                   {-1, 1},
                                   {0,  1},
                                   {1,  1},
                                   {1,  0},
                                   {1,  -1},
                                   {0,  -1},
                                   {-1, -1}};
            for (int l = 0; l < 8; ++l) {
                int d0 = direction[l][0];
                int d1 = direction[l][1];
                if (B(x + d0, y + d1))
                    king[i] |= 1ULL << (x + d0 + 8 * (y + d1));
            }
            //rook
            for (ull block = 0; block < 256; block++) {
                int nd = y;
                ull mask = block << 8 * nd;
                for (int j = 0; j < 2; j++) {
                    int dir[2][2] = {{1,  0},
                                     {-1, 0}};
                    int k = 1;
                    while (B(x + k * dir[j][0], y + k * dir[j][1]) &&
                           ((mask >> (8 * (y + k * dir[j][1]) + x + k * dir[j][0])) & 1) == 0) {
                        rook_G[i][(int) block] |= 1ULL << (8 * (y + k * dir[j][1]) + x + k * dir[j][0]);
                        k++;
                    }
                    if (((mask >> (8 * (y + k * dir[j][1]) + x + k * dir[j][0])) & 1) != 0 &&
                        B(x + k * dir[j][0], y + k * dir[j][1]))
                        rook_G[i][(int) block] |= 1ULL << (8 * (y + k * dir[j][1]) + x + k * dir[j][0]);
                }

                nd = 7 - x;
                mask = rotate_90(block << 8 * nd);
                for (int j = 0; j < 2; j++) {
                    int dir[2][2] = {{0, 1},
                                     {0, -1}};
                    int k = 1;
                    while (B(x + k * dir[j][0], y + k * dir[j][1]) &&
                           ((mask >> (8 * (y + k * dir[j][1]) + x + k * dir[j][0])) & 1) == 0) {
                        rook_V[i][(int) block] |= 1ULL << (8 * (y + k * dir[j][1]) + x + k * dir[j][0]);
                        k++;
                    }
                    if (((mask >> (8 * (y + k * dir[j][1]) + x + k * dir[j][0])) & 1) != 0 &&
                        B(x + k * dir[j][0], y + k * dir[j][1]))
                        rook_V[i][(int) block] |= 1ULL << (8 * (y + k * dir[j][1]) + x + k * dir[j][0]);
                }

            }
            //bishop
            for (ull block = 0; block < 256; block++) {
                int nd = y - x + 7;
                ull mask = rotate_L(block << ((nd >= 9) ? ((21 - nd) * (nd - 8) / 2 + 36) : (nd * (nd + 1) / 2)));
                for (int j = 0; j < 2; j++) {
                    int dir[2][2] = {{1,  1},
                                     {-1, -1}};
                    int k = 1;
                    while (B(x + k * dir[j][0], y + k * dir[j][1]) &&
                           ((mask >> (8 * (y + k * dir[j][1]) + x + k * dir[j][0])) & 1) == 0) {
                        bishop_L[i][(int) block] |= 1ULL << (8 * (y + k * dir[j][1]) + x + k * dir[j][0]);
                        k++;
                    }
                    if (((mask >> (8 * (y + k * dir[j][1]) + x + k * dir[j][0])) & 1) != 0 &&
                        B(x + k * dir[j][0], y + k * dir[j][1]))
                        bishop_L[i][(int) block] |= 1ULL << (8 * (y + k * dir[j][1]) + x + k * dir[j][0]);
                }

                nd = y + x;
                mask = rotate_R(block << ((nd >= 9) ? ((21 - nd) * (nd - 8) / 2 + 36) : (nd * (nd + 1) / 2)));
                for (int j = 0; j < 2; j++) {
                    int dir[2][2] = {{-1, 1},
                                     {1,  -1}};
                    int k = 1;
                    while (B(x + k * dir[j][0], y + k * dir[j][1]) &&
                           ((mask >> (8 * (y + k * dir[j][1]) + x + k * dir[j][0])) & 1) == 0) {
                        bishop_R[i][(int) block] |= 1ULL << (8 * (y + k * dir[j][1]) + x + k * dir[j][0]);
                        k++;
                    }
                    if (((mask >> (8 * (y + k * dir[j][1]) + x + k * dir[j][0])) & 1) != 0 &&
                        B(x + k * dir[j][0], y + k * dir[j][1]))
                        bishop_R[i][(int) block] |= 1ULL << (8 * (y + k * dir[j][1]) + x + k * dir[j][0]);
                }
            }
            i++;
        }

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            cell_rotated_90[8 * y + x] |= 1ULL << 8 * (7 - x) + y;
        }
    int n = 0;
    for (int i2 = 0; i2 < 8; i2++) {
        int m = 0;
        for (int j = 7 - i2; j <= 7; j++) {
            cell_rotated_45_left[j + 8 * m] |= 1ULL << n;
            m++;
            n++;
        }
    }
    for (int i2 = 8; i2 < 16; i2++) {
        int m = 0;
        for (int j = 7 - i2; j <= 7; j++) {
            if (j + 8 * m > -1 && j + 8 * m < 64 && (j + 8 * m) / 8 > (j + 8 * m) % 8) {
                cell_rotated_45_left[j + 8 * m] |= 1ULL << n;
                n++;
            }
            m++;
        }
    }


    n = 0;
    for (int i2 = 0; i2 < 8; i2++) {
        int m = 8 * i2;
        for (int j = 0; j <= i2; j++) {
            cell_rotated_45_right[j + m] |= 1ULL << n;
            m -= 8;
            n++;
        }
    }
    for (int i2 = 8; i2 < 16; i2++) {
        int m = 8 * i2;
        for (int j = 0; j <= i2; j++) {
            if (j + m > -1 && j + m < 64 && (j + m) / 8 + (j + m) % 8 > 7) {
                cell_rotated_45_right[j + m] |= 1ULL << n;
                n++;
                if (j + m == 63)
                    return;
            }
            m -= 8;
        }
    }
}

