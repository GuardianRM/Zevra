#include "game.hpp"

int Game::startGame() {
	std::string str;
	uciHandler("position startpos");	

	while(true) {
		std::getline(std::cin, str);

		if(!uciHandler(str)) {
			return 0;
		};
	}
}

void Game::goFixedDepth() {
	++hashAge;
	
	if(option.UCI_AnalyseMode || abs(bestScore) >= (WHITE_WIN - 100)) {
		clearCash();
	}
	
	stopped = false;
	std::vector<uint64_t>hash;

	nodesCounter = 0;
	int max_depth_global = max_depth;
	max_depth = 1;

	start_timer = clock();
	hasBestMove = false;

	BitMove moveCritical = game_board.getRandomMove();
	bestMove = moveCritical;
	hasBestMove = true;
	bestScore = 0;

	for(; max_depth <= max_depth_global; ++max_depth) {
		flattenHistory();

		negamax(game_board, -WHITE_WIN, WHITE_WIN, max_depth, 0, FIXED_DEPTH, false, true);

		hasBestMove = true;

		if((abs(bestScore) >= (WHITE_WIN - 100) && max_depth_global < 99) || stopped) {
			break;
		}
	}

	end_timer = clock();

	if(hasBestMove) {
		std::cout << "bestmove " << bestMove.getMoveString() << std::endl;
	}
}

void Game::goFixedTime(int64_t tm, bool tournamentTimeControll) {
	tm -= option.moveOverhead;
	tm = std::max(tm, (int64_t)10);

	++hashAge;
	stopped = false;

	time = tm;
	timer.start();

	if(option.UCI_AnalyseMode || abs(bestScore) >= (WHITE_WIN - 100)) {
		clearCash();
	}

	std::vector<uint64_t>hash;

	nodesCounter = 0;

	start_timer = clock();
	hasBestMove = false;

	BitMove moveCritical = game_board.getRandomMove();
	bestMove = moveCritical;
	bestScore = 0;
	hasBestMove = true;
	max_depth = 1;

	std::vector<BitMove> bestPV;

	int f = 0;


	//4k3/8/8/8/8/8/8/4KNNN w - - 0 1

	for(; timer.getTime() < time; ) {
		flattenHistory();

		if(tournamentTimeControll) {
			/*if(timer.getTime() * 2 >= time) {
				break;
			}*/

			if(max_depth == 1) {
				game_board.bitBoardMoveGenerator(moveArray[0], stress);
				BitMove mv;
				uint8_t color;
				if(game_board.currentState.whiteMove) {
					color = WHITE;
				} else {
					color = BLACK;
				}
				int count = 0;
				for(unsigned int i = 0; i < moveArray[0].moveArray.size(); ++i) {
					game_board.move(moveArray[0].moveArray[i]);
					if(!game_board.inCheck(color)) {
						++count;
						mv = moveArray[0].moveArray[i];
					}
					
					game_board.goBack();
				}
				
				if(count == 1) {
					bestMove = mv;
					break;
				}
			}
		}



		//f = negamax(game_board, -WHITE_WIN, WHITE_WIN, max_depth, 0, FIXED_TIME, false, true);
		int upperbound = WHITE_WIN;
		int lowerbound = -WHITE_WIN;

		/*for(;;) {
			f = negamax(game_board, alpha, beta, max_depth, 0, FIXED_TIME, false, true);
			if(f <= alpha)  {
				alpha = -WHITE_WIN;
			} else if(f >= beta) {
				beta = WHITE_WIN;
			} else {
				break;
			}
		}*/

		while(lowerbound < upperbound) {
			int b = std::max(f, lowerbound + 1);
			f = negamax(game_board, b - 1, b, max_depth, 0, FIXED_TIME, false, true);

			if(f < b) {
				upperbound = f;
			} else {
				lowerbound = f;
			}
		}

		
		if(stopped) {
			break;
		}

		hasBestMove = true;
		++max_depth;
	}

	end_timer = clock();

	if(hasBestMove) {
		std::cout << "bestmove " << bestMove.getMoveString() << std::endl;
	}
}

void Game::goTournament() {
	double tm, inc;
	if(game_board.currentState.whiteMove) {
		tm = wtime;
		inc = winc;
	} else {
		tm = btime;
		inc = binc;
	}

	double k;

	int figuresNumber = game_board.popcount64(game_board.currentState.white_bit_mask | game_board.currentState.black_bit_mask);

	if(movestogoEnable) {
		k = movestogo;
		goFixedTime(tm / (k + 1) + inc / 2, true);
	} else {
		k = 40 - (32 - figuresNumber);
		goFixedTime(tm / k + inc / 2 - 100, true);
	}
}

bool Game::move(std::string mv) {
	MoveArray moves;
	game_board.bitBoardMoveGenerator(moves, stress);
	for(unsigned int i = 0; i < moves.count; ++i) {
		if(moves.moveArray[i].getMoveString() == mv) {
			game_board.move(moves.moveArray[i]);
			uint8_t color;

			if(game_board.currentState.whiteMove) {
				color = BLACK;
			} else {
				color = WHITE;
			}

			if(game_board.inCheck(color)) {
				game_board.goBack();
				continue;
			}

			++hash_decrement;

			return true;
		}
	}

	return false;
}
