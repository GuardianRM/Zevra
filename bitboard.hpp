#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include "constants.hpp"
#include "movearray.hpp"
#include "goback.hpp"

class BitBoard {
private:
	uint64_t figures[7];
	uint64_t horizontal[8];
	uint64_t vertical[8];
	uint64_t white_bit_mask, black_bit_mask;

	uint8_t moveNumber, ruleNumber, passant_x, passant_y;
	void preInit();
	std::vector<std::string> splitter(std::string str, char sym);

	uint64_t plus1[64], plus7[64], plus8[64], plus9[64], minus1[64], minus7[64], minus8[64], minus9[64];
	uint64_t vec2_cells[8][8];
	uint64_t vec1_cells[64];
	uint64_t bitboard[32][8][8];

	uint8_t popcount64(uint64_t value);
	uint8_t firstOne(uint64_t mask);
	uint8_t lastOne(uint64_t mask);
	void clearCell(uint8_t y, uint8_t x);
	void printBitBoard(uint64_t bit_board);

	std::stack<GoBack> history;
public:
	BitBoard();
	~BitBoard();

	void setFen(std::string fen);
	std::string getFen();
	void clear();
	int stress;

	void bitBoardMoveGenerator(MoveArray& moveArray);
	void move(BitMove& mv);
	void goBack();
	void pushHistory();

	bool whiteMove, wsc, wlc, bsc, blc, passant_enable;
};

#endif
