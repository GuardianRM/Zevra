#include "hash.hpp"

Hash::Hash() :/* prev(nullptr),*/ age(0) {
	clean();
}

Hash::~Hash() {
	clean();
}
void Hash::clean() {
	flag = EMPTY;
	lowerbound = -WHITE_WIN;
	upperbound = WHITE_WIN;
}

void Hash::setMove(BitMove& mv) {
	fromY = mv.fromY;
	fromX = mv.fromX;
	toY = mv.toY;
	toX = mv.toX;
	replaced = mv.replaced;
	replacedFigure = mv.replacedFigure;
}

bool Hash::move_equal(BitMove& mv) {
	//return mv.equal(move);
	return (mv.fromY == fromY && mv.fromX == fromX && mv.toY == toY && mv.toX == toX && (!mv.replaced || mv.replacedFigure == replacedFigure));
}

BitMove Hash::getMove() {
	BitMove result = BitMove(0, fromY, fromX, toY, toX);
	result.setReplaced(replacedFigure);
	return result;
}