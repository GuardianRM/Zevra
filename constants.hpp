#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cstdint>
#include <cwchar>

const uint8_t PAWN = 1;    //00000001
const uint8_t KNIGHT = 2;  //00000010
const uint8_t BISHOP = 3;  //00000011
const uint8_t ROOK = 4;    //00000100
const uint8_t QUEEN = 5;   //00000101
const uint8_t KING = 6;    //00000110

const uint8_t WHITE = 8;   //00001000
const uint8_t BLACK = 16;  //00010000

const uint8_t COLOR_SAVE = 24;  //00011000
const uint8_t TYPE_SAVE = 7; //00000111

const int BOARD_SIZE = 8;

const uint8_t STANDARD_MV = 0;
const uint8_t WS_CASTLING_MV = 1;
const uint8_t WL_CASTLING_MV = 2;
const uint8_t BS_CASTLING_MV = 3;
const uint8_t BL_CASTLING_MV = 4;
const uint8_t PASSANT_MV = 5;
const uint8_t TRANSFORMATION_MV = 6;

const double PAWN_EV = 1000;
const double KNIGHT_EV = 3200;
const double BISHOP_EV = 3300;
const double ROOK_EV = 5000;
const double QUEEN_EV = 9000;

const int hash_width = 24;

const double ALL_MATERIAL = 16 * PAWN_EV
+	4 * KNIGHT_EV
+	4 * BISHOP_EV
+	4*ROOK_EV
+	QUEEN_EV;

const double whitePawnMatr[BOARD_SIZE][BOARD_SIZE] = {
{0, 0, 0, 0, 0, 0, 0, 0},
{50, 50, 50, 50, 50, 50, 50, 50},
{10, 10, 20, 30, 30, 20, 10, 10},
{5, 5, 10, 25, 25, 10, 5, 5},
{0, 0, 0, 20, 20, 0, 0, 0},
{5, -5, -10, 0, 0, -10, -5, 5},
{5, 10, 10, -20, -20, 10, 10, 5},
{0, 0, 0, 0, 0, 0, 0, 0}
};

const double knightMatr[BOARD_SIZE][BOARD_SIZE] {
{-50, -40, -30, -30, -30, -30, -40, -50},
{-40, -20, 0, 0, 0, 0, -20, -40},
{-30, 0, 10, 15, 15, 10, 0, -30},
{-30, 5, 15, 20, 20, 15, 5, -30},
{-30, 0, 15, 20, 20, 15, 5, -30},
{-30, 5, 10, 15, 15, 10, 0, -30},
{-40, -20, 0, 5, 5, 0, -20, -40},
{-50, -40, -30, -30, -30, -30, -40, -50}
};

const double bishopMatr[BOARD_SIZE][BOARD_SIZE] {
{-20, -10, -10, -10, -10, -10, -10, -20},
{-10, 0, 0, 0, 0, 0, 0, -10},
{-10, 0, 5, 10, 10, 5, 0, -10},
{-10, 5, 5, 10, 10, 5, 5, -10},
{-10, 0, 10, 10, 10, 10, 0, -10},
{-10, 10, 10, 10, 10, 10, 10, -10},
{-10, 5, 0, 0, 0, 0, 5, -10},
{-20, -10, -10, -10, -10, -10, -10, -20}
};

const double rookMatr[BOARD_SIZE][BOARD_SIZE] {
{0, 0, 0, 0, 0, 0, 0, 0},
{5, 10, 10, 10, 10, 10, 10, 5},
{-5, 0, 0, 0, 0, 0, 0, -5},
{-5, 0, 0, 0, 0, 0, 0, -5},
{-5, 0, 0, 0, 0, 0, 0, -5},
{-5, 0, 0, 0, 0, 0, 0, -5},
{-5, 0, 0, 0, 0, 0, 0, -5},
{0, 0, 0, 5, 5, 0, 0, 0}
};

const double queenMatr[BOARD_SIZE][BOARD_SIZE] {
{-20, -10, -10, -5, -5, -10, -10, -20},
{-10, 0, 0, 0, 0, 0, 0, -10},
{-10, 0, 5, 5, 5, 5, 0, -10},
{-5, 0, 5, 5, 5, 5, 0, -5},
{0, 0, 5, 5, 5, 5, 0, -5},
{-10, 0, 5, 0, 0, 0, 0, -10},
{-20, -10, -10, -5, -5, -10, -10, -20}
};

const double kingDebuteMatr[BOARD_SIZE][BOARD_SIZE] {
{-30, -40, -40, -50, -50, -40, -40, -30},
{-30, -40, -40, -50, -50, -40, -40, -30},
{-30, -40, -40, -50, -50, -40, -40, -30},
{-30, -40, -40, -50, -50, -40, -40, -30},
{-20, -30, -30, -40, -40, -30, -30, -20},
{-10, -20, -20, -20, -20, -20, -20, -10},
{20, 20, 0, 0, 0, 0, 20, 20},
{20, 30, 10, 0, 0, 10, 30, 20}
};

const double kingEndGameMatr[BOARD_SIZE][BOARD_SIZE] {
{-50, -40, -30, -20, -20, -30, -40, -50},
{-30, -20, -10, 0, 0, -10, -20, -30},
{-30, -10, 20, 30, 30, 20, -10, -30},
{-30, -10, 30, 40, 40, 30, -10, -30},
{-30, -10, 30, 40, 40, 30, -10, -30},
{-30, -10, 20, 30, 30, 20, -10, -30},
{-50, -30, -30, -30, -30, -30, -30, -50}
};

#endif
