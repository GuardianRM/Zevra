#include "game.hpp"

int Game::startGame() {
	std::string str;
	while(true) {
		std::getline(std::cin, str);

		if(!uciHandler(str)) {
			return 0;
		};
	}
}

void Game::goFixedDepth() {
	clearCash();

	variant.clear();
	variant.resize(max_depth);
	std::vector<uint64_t>hash;

	std::vector<BitMove>pv;
	whiteUp = BLACK_WIN;
	blackUp = WHITE_WIN;

	bool basis = false;
	movesCounter = 0;
	int max_depth_global = max_depth;
	max_depth = 1;

	start_timer = clock();
	hasBestMove = false;
	double depth;

	for(; max_depth <= max_depth_global; ++max_depth) {
		pv.resize(0);
		whiteUp = BLACK_WIN;
		blackUp = WHITE_WIN;
		flattenHistory();
		if(max_depth == max_depth_global) {
			basis = true;
		}


		negamax(game_board, -INFINITY, INFINITY, max_depth, 0, FIXED_DEPTH);
		hasBestMove = true;

		if(abs(bestScore) >= (WHITE_WIN - 100)) {
			break;
		}

		depth = max_depth;
	}

	end_timer = clock();

	std::cout << "info depth " << depth << " ";
	printScore(bestScore);
	std::cout << " nodes " << movesCounter << " nps " << (int)(movesCounter / ((end_timer - start_timer) / CLOCKS_PER_SEC)) <<
	" time " << (int)((end_timer - start_timer) / (CLOCKS_PER_SEC / 1000)) << "\n";
	if(hasBestMove) {
		std::cout << "bestmove " << bestMove.getMoveString() << "\n";
	}
}

void Game::goFixedTime(int tm) {
	time = tm;
	timer.start();

	clearCash();

	variant.clear();
	variant.resize(max_depth);
	std::vector<uint64_t>hash;

	std::vector<BitMove>pv;
	whiteUp = BLACK_WIN;
	blackUp = WHITE_WIN;

	bool basis = false;
	movesCounter = 0;

	start_timer = clock();
	hasBestMove = false;
	double depth;
	for(max_depth = 1; timer.getTime() < time; ++max_depth) {
		pv.resize(0);
		whiteUp = BLACK_WIN;
		blackUp = WHITE_WIN;
		flattenHistory();

		negamax(game_board, -INFINITY, INFINITY, max_depth, 0, FIXED_TIME);

		if(abs(bestScore) >= (WHITE_WIN - 100)) {
			break;
		}

		hasBestMove = true;
		depth = max_depth;
	}

	end_timer = clock();

	std::cout << "info depth " << depth << " ";
	printScore(bestScore);
	std::cout << " nodes " << movesCounter << " nps " << (int)(movesCounter / ((end_timer - start_timer) / CLOCKS_PER_SEC)) <<
	" time " << (int)((end_timer - start_timer) / (CLOCKS_PER_SEC / 1000)) << "\n";
	if(hasBestMove) {
		std::cout << "bestmove " << bestMove.getMoveString() << "\n";
	}
}

bool Game::move(std::string mv) {
	MoveArray moves;
	game_board.bitBoardMoveGenerator(moves);
	for(unsigned int i = 0; i < moves.count; ++i) {
		if(moves.moveArray[i].getMoveString() == mv) {
			game_board.move(moves.moveArray[i]);
			uint8_t color;

			if(game_board.whiteMove) {
				color = BLACK;
			} else {
				color = WHITE;
			}

			if(game_board.inCheck(color)) {
				game_board.goBack();
				continue;
			}

			return true;
		}
	}

	return false;
}
