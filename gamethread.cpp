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
	//clearCash();

	variant.clear();
	variant.resize(max_depth);
	std::vector<uint64_t>hash;

	pv_best.clear();
	std::vector<Move>pv;
	whiteUp = BLACK_WIN;
	blackUp = WHITE_WIN;

	bool basis = false;
	movesCounter = 0;
	//boardHash.clear();
	int max_depth_global = max_depth;

	if(max_depth_global % 2 == 0) {
		max_depth = 2;
	} else {
		max_depth = 1;
	}

	start_timer = clock();
	hasBestMove = false;
	double depth;
	for(; max_depth <= max_depth_global; max_depth += 2) {
		pv_tmp.resize(0);
		pv_best.resize(0);
		flattenHistory();
		//cleanWhiteHistory();
		//cleanBlackHistory();
		if(max_depth == max_depth_global) {
			basis = true;
		}

		/*if(game_board.isWhiteMove()) {
			minimax_white(game_board, -INFINITY, INFINITY, 0, 0, gameHash, basis, pv, true, FIXED_DEPTH);
		} else {
			minimax_black(game_board, -INFINITY, INFINITY, 0, 0, gameHash, basis, pv, true, FIXED_DEPTH);
		}*/

		negamax(game_board, -INFINITY, INFINITY, 0, 0, gameHash, true, pv, true, FIXED_DEPTH, false);


		/*std::cout << "info pv ";
		for(unsigned int i = 0; i < pv_best.size(); ++i) {
			std::cout << pv_best[i].getMoveString() << " ";
		}
		std::cout << "\n";*/

		if(bestScore + 1000 >= WHITE_WIN || bestScore - 1000 <= BLACK_WIN) {
			break;
		}

		depth = max_depth;
		hasBestMove = true;
	}

	end_timer = clock();

	std::cout << "info depth " << depth << " ";
	printScore(bestScore);
	std::cout << " pv ";
	for(unsigned int i = 0; i < pv_best.size(); ++i) {
		std::cout << pv_best[i].getMoveString() << " ";
	}
	std::cout << "nodes " << movesCounter << " nps " << (int)(movesCounter / ((end_timer - start_timer) / CLOCKS_PER_SEC)) <<
	" time " << (int)((end_timer - start_timer) / (CLOCKS_PER_SEC / 1000)) << "\n";
	if(hasBestMove) {
		std::cout << "bestmove " << bestMove.getMoveString() << "\n";
	}
}

void Game::goFixedTime(int tm) {
	time = tm;
	timer.start();

	//clearCash();

	variant.clear();
	variant.resize(max_depth);
	std::vector<uint64_t>hash;

	pv_best.clear();
	std::vector<Move>pv;
	whiteUp = BLACK_WIN;
	blackUp = WHITE_WIN;

	bool basis = false;
	movesCounter = 0;

	start_timer = clock();
	hasBestMove = false;
	double depth;
	for(max_depth = 1; timer.getTime() < time; ++max_depth) {
		pv_tmp.resize(0);
		pv_best.resize(0);
		flattenHistory();

		/*if(game_board.isWhiteMove()) {
			minimax_white(game_board, -INFINITY, INFINITY, 0, 0, gameHash, basis, pv, true, FIXED_TIME);
		} else {
			minimax_black(game_board, -INFINITY, INFINITY, 0, 0, gameHash, basis, pv, true, FIXED_TIME);
		}*/
		negamax(game_board, -INFINITY, INFINITY, 0, 0, gameHash, true, pv, true, FIXED_TIME, false);

		/*std::cout << "info pv ";
		for(unsigned int i = 0; i < pv_best.size(); ++i) {
			std::cout << pv_best[i].getMoveString() << " ";
		}
		std::cout << "\n";*/

		if(bestScore + 1000 >= WHITE_WIN || bestScore - 1000 <= BLACK_WIN) {
			break;
		}
		depth = max_depth;
		hasBestMove = true;
	}

	end_timer = clock();

	//std::cout << "bestmove " << bestMove.getMoveString() << " time " <<  "\n";

	std::cout << "info depth " << depth << " ";
	printScore(bestScore);
	std::cout << " pv ";
	for(unsigned int i = 0; i < pv_best.size(); ++i) {
		std::cout << pv_best[i].getMoveString() << " ";
	}
	std::cout << "nodes " << movesCounter << " nps " << (int)(movesCounter / ((end_timer - start_timer) / CLOCKS_PER_SEC)) <<
	" time " << (int)((end_timer - start_timer) / (CLOCKS_PER_SEC / 1000)) << "\n";
	if(hasBestMove) {
		std::cout << "bestmove " << bestMove.getMoveString() << "\n";
	}
}

bool Game::move(std::string mv) {
	bool tmp_shah = false;
	std::vector<Move>moves = generatePositionMoves(game_board, tmp_shah, true, 0);

	for(unsigned int i = 0; i < moves.size(); ++i) {
		if(moves[i].getMoveString() == mv) {
			Board tmp = game_board;
			game_board.move(moves[i]);
			bool shah = false;
			std::vector<Move>moves = generatePositionMoves(game_board, shah, true, 0);

			if(shah) {
				game_board = tmp;
				continue;
			}

			gameHash.push_back(getHash(game_board));

			return true;
		}
	}

	return false;
}
