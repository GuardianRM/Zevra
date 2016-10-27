#ifndef GOBACK_HPP
#define GOBACK_HPP

#include <cstdint>
#include "constants.hpp"
#include "figurecell.hpp"

class GoBack {
public:
	GoBack();
 	uint64_t figures[7];
	uint64_t white_bit_mask, black_bit_mask, castlingMap;
	uint8_t moveNumber, ruleNumber, passant_x, passant_y;
	bool whiteMove, passant_enable;
	double evalute;
};

#endif
