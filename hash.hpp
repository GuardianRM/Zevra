#ifndef HASH_HPP
#define HASH_HPP

#include <cstdint>
#include "bitmove.hpp"

class BitMove;

class Hash {
public:
	uint8_t depth;
	int64_t score;
	uint8_t flag;
	uint64_t key;
	BitMove move;

	Hash();
};

#endif
