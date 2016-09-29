#include "game.hpp"

double Game::negamax(Board b, double alpha, double beta, int depth, int real_depth, std::vector<uint64_t> hash, bool basis, std::vector<Move>pv, bool usedNullMove, int rule) {
	if(rule == FIXED_TIME && timer.getTime() >= time) {
		return 0;
	}

	++movesCounter;

	uint8_t color;
	if(b.isWhiteMove()) {
		color = WHITE;
	} else {
		color = BLACK;
	}

	uint64_t pos_hash = getHash(b);
	uint64_t board_hash = getColorHash(b, pos_hash);

	if(depth > 0) {
	int third_repeat = 1;
		for(unsigned int i = 0; i < hash.size(); ++i) {
			if(hash[i] == pos_hash) {
				++third_repeat;
			}
			if(third_repeat >= 3  || b.move_rule_num >= 50) {
				return 0;
			}
		}
	}

	if(depth >= max_depth) {
		return quies(b, alpha, beta, rule);
	}

/*	if(inCheck(b, color) && depth > 0) {
		--depth;
		basis = false;
	}*/

	int num_moves = 0;

	double max = BLACK_WIN;

	//bool null_mv = nullMoveEnable;

	bool tmp_shah;
	std::vector<Move>moves = generatePositionMoves(b, tmp_shah, true, real_depth);

	Move local_move;

	if(moves.size() > 0) {
		local_move = moves[0];
	}

	for(unsigned int i = 0; i < moves.size(); ++i) {
		if(rule == FIXED_TIME) {
			if(timer.getTime() >= time) {
				return 0;
			}
		}
		Board tmp_brd = b;
		tmp_brd.move(moves[i]);

		if(inCheck(tmp_brd, color)) {
			continue;
		}

		if(num_moves == 0) {
			local_move = moves[i];
		}

		++num_moves;

		double tmp;

		pv.push_back(moves[i]);
		hash.push_back(pos_hash);

		if(moves[i].fromHash && hashEnable) {
			if(boardHash[board_hash & hash_cutter].type_mv == BETA_CUT_EV) {
				alpha = boardHash[board_hash & hash_cutter].evalute;
			}
		}

		/*if(num_moves <= 3 || inCheck(tmp_brd, BLACK) || depth < 3) {
				tmp = minimax_black(tmp_brd, alpha, beta, depth + 1, max_depth, real_depth + 1, hash, basis, pv, true);
		} else {
			tmp = minimax_black(tmp_brd, alpha+1, beta, depth + 2, max_depth, real_depth + 2, hash, basis, pv, true);
			if(tmp > alpha) {
				//tmp_brd = b;
				tmp = minimax_black(tmp_brd, alpha, beta, depth + 1, max_depth, real_depth + 1, hash, basis, pv, true);
			}
		}*/

		//if(num_moves == 1) {
				tmp = -negamax(tmp_brd, -beta, -alpha, depth + 1, real_depth + 1, hash, basis, pv, true, rule);
	/*	} else {
			tmp = -negamax(tmp_brd, -beta, -alpha + 1, depth + 1, real_depth + 1, hash, basis, pv, true, rule);

			if(tmp > alpha && tmp < beta) {
				tmp = -negamax(tmp_brd, -beta, -alpha, depth + 1, real_depth + 1, hash, basis, pv, true, rule);
			}
		}*/

		//tmp = minimax_black(tmp_brd, alpha, beta, depth + 1, real_depth + 1, hash, basis, pv, true, rule);

		pv.pop_back();

		if(depth == 0) {
			std::cout << "info score cp " << (int) (max / PAWN_EV * 100) << " depth " << max_depth << "\n";
			std::cout << "info currmove " << moves[i].getMoveString() << " currmovenumber " << num_moves << "\n";
		}

		if(tmp > max) {
			max = tmp;
			local_move = moves[i];

			if(pv_tmp.size() < (unsigned int) (depth + 1)) {
				pv_tmp.resize(depth + 1);
			}
			pv_tmp[depth] = local_move;

			if(depth == 0) {
				pv_best = pv_tmp;
				pv_tmp.resize(1);
			}

		}

		if(tmp > alpha) {
			alpha = tmp;
		}
		if(max >= beta) {
			if(hashEnable) {
				if(!boardHash[board_hash & hash_cutter].enable) {
					boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, max, BETA_CUT_EV);
				} else {
					if(boardHash[board_hash & hash_cutter].depth <= max_depth - real_depth) {
						boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, max, BETA_CUT_EV);
					}
				}
			}

			if(b.board[moves[i].toY][moves[i].toX] == 0) {
				if(color == WHITE) {
					whiteHistorySort[moves[i].fromY][moves[i].fromX][moves[i].toY][moves[i].toX] += pow(max_depth - real_depth, max_depth - real_depth);
				} else {
					blackHistorySort[moves[i].fromY][moves[i].fromX][moves[i].toY][moves[i].toX] += pow(max_depth - real_depth, max_depth - real_depth);
				}
			}

			if(depth == 0) {
				bestmove = local_move;
				bestMove = bestmove;
				std::cout << "info nodes " << movesCounter;
				//if(basis || rule == FIXED_TIME) {
					std::cout << " score ";
					if(max > BLACK_WIN + 10000 && max < WHITE_WIN - 10000) {
						std::cout << "cp " << (int) (max / PAWN_EV * 100) << "\n";
					} else if(max < 0) {
						std::cout << "mate " <<  -abs(max - BLACK_WIN) / 2 - 1 << "\n";
					} else {
						std::cout << "mate " <<  abs(max - WHITE_WIN) / 2 + 1 << "\n";
					}

					/*if(basis) {
						std::cout << "\nbestmove " << local_move.getMoveString();
						gameHash.push_back(getHash(game_board));
					}*/
					gameHash.push_back(getHash(game_board));
				}
			//}

			return max;
		}
	}



	if(num_moves == 0) {
		if(inCheck(b, color)) {
			return BLACK_WIN + real_depth;
		} else {
			return 0;
		}
	}

	if(depth == 0 && num_moves > 0) {
		bestmove = local_move;
		bestMove = bestmove;
		std::cout << "info nodes " << movesCounter;
		//if(basis || rule == FIXED_TIME) {
			std::cout << " score ";
			if(max > BLACK_WIN + 10000 && max < WHITE_WIN - 10000) {
				std::cout << "cp " << (int) (max / PAWN_EV * 100) << "\n";
			} else if(max < 0) {
				std::cout << "mate " <<  -abs(max - BLACK_WIN) / 2 - 1 << "\n";
			} else {
				std::cout << "mate " <<  abs(max - WHITE_WIN) / 2 + 1 << "\n";
			}

			/*if(basis) {
				std::cout << "\nbestmove " << local_move.getMoveString();

			}*/
			gameHash.push_back(getHash(game_board));
	//	}
	}

	printVariant();

	if(hashEnable) {
		if(!boardHash[board_hash & hash_cutter].enable) {
			boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, max, REAL_EV);
		} else {
			if(boardHash[board_hash & hash_cutter].depth <= max_depth - real_depth) {
				boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, max, REAL_EV);
			}
		}
	}

	return max;
}

double Game::minimax_white(Board b, double alpha, double beta, int depth, int real_depth, std::vector<uint64_t> hash, bool basis, std::vector<Move>pv, bool usedNullMove, int rule) {
	if(rule == FIXED_TIME && timer.getTime() >= time) {
		return 0;
	}

	++movesCounter;

	if(depth >= max_depth) {
		return evalute(b);
		//return quies(b, alpha, beta);
	}

	uint64_t pos_hash = getHash(b);
	uint64_t board_hash = getColorHash(b, pos_hash);

	if(depth > 0) {
	int third_repeat = 1;
		for(unsigned int i = 0; i < hash.size(); ++i) {
			if(hash[i] == pos_hash) {
				++third_repeat;
			}
			if(third_repeat >= 3  || b.move_rule_num >= 50) {
				return 0;
			}
		}
	}

	int num_moves = 0;

	double max = BLACK_WIN;

	//bool null_mv = nullMoveEnable;

	bool tmp_shah;
	std::vector<Move>moves = generatePositionMoves(b, tmp_shah, true, real_depth);

	Move local_move;

	if(moves.size() > 0) {
		local_move = moves[0];
	}

	for(unsigned int i = 0; i < moves.size(); ++i) {
		if(rule == FIXED_TIME) {
			if(timer.getTime() >= time) {
				return 0;
			}
		}
		Board tmp_brd = b;
		tmp_brd.move(moves[i]);

		if(inCheck(tmp_brd, WHITE)) {
			continue;
		}

		if(num_moves == 0) {
			local_move = moves[i];
		}

		++num_moves;

		double tmp;

		pv.push_back(moves[i]);
		hash.push_back(pos_hash);

		if(moves[i].fromHash && hashEnable) {
			if(boardHash[board_hash & hash_cutter].type_mv == BETA_CUT_EV) {
				alpha = boardHash[board_hash & hash_cutter].evalute;
			}
		}

		/*if(num_moves <= 3 || inCheck(tmp_brd, BLACK) || depth < 3) {
				tmp = minimax_black(tmp_brd, alpha, beta, depth + 1, max_depth, real_depth + 1, hash, basis, pv, true);
		} else {
			tmp = minimax_black(tmp_brd, alpha+1, beta, depth + 2, max_depth, real_depth + 2, hash, basis, pv, true);
			if(tmp > alpha) {
				//tmp_brd = b;
				tmp = minimax_black(tmp_brd, alpha, beta, depth + 1, max_depth, real_depth + 1, hash, basis, pv, true);
			}
		}*/

		if(num_moves == 1) {
				tmp = minimax_black(tmp_brd, alpha, beta, depth + 1, real_depth + 1, hash, basis, pv, true, rule);
		} else {
			tmp = minimax_black(tmp_brd, alpha, alpha + 1, depth + 1, real_depth + 1, hash, basis, pv, true, rule);

			if(tmp > alpha && tmp < beta) {
				tmp = minimax_black(tmp_brd, alpha, beta, depth + 1, real_depth + 1, hash, basis, pv, true, rule);
			}
		}

		//tmp = minimax_black(tmp_brd, alpha, beta, depth + 1, real_depth + 1, hash, basis, pv, true, rule);

		pv.pop_back();

		if(depth == 0) {
			std::cout << "info score cp " << (int) (max / PAWN_EV * 100) << " depth " << max_depth << "\n";
			std::cout << "info currmove " << moves[i].getMoveString() << " currmovenumber " << num_moves << "\n";
		}

		if(tmp > max) {
			max = tmp;
			local_move = moves[i];

			if(pv_tmp.size() < (unsigned int) (depth + 1)) {
				pv_tmp.resize(depth + 1);
			}
			pv_tmp[depth] = local_move;

			if(depth == 0) {
				pv_best = pv_tmp;
				pv_tmp.resize(1);
			}

		}

		if(tmp > alpha) {
			alpha = tmp;
			if(b.board[moves[i].toY][moves[i].toX] == 0) {
				whiteKiller[real_depth] = Killer(local_move);
			}
		}
		if(max >= beta) {
			if(hashEnable) {
				if(!boardHash[board_hash & hash_cutter].enable) {
					boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, max, BETA_CUT_EV);
				} else {
					if(boardHash[board_hash & hash_cutter].depth <= max_depth - real_depth) {
						boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, max, BETA_CUT_EV);
					}
				}
			}

			if(b.board[moves[i].toY][moves[i].toX] == 0) {
				whiteHistorySort[moves[i].fromY][moves[i].fromX][moves[i].toY][moves[i].toX] += pow(max_depth - real_depth, max_depth - real_depth);
			}

			if(depth == 0) {
				bestmove = local_move;
				bestMove = bestmove;
				std::cout << "info pv " << local_move.getMoveString();
				std::cout << " nodes " << movesCounter;
				if(basis || rule == FIXED_TIME) {
					std::cout << " score ";
					if(max > BLACK_WIN + 10000 && max < WHITE_WIN - 10000) {
						std::cout << "cp " << (int) (max / PAWN_EV * 100);
					} else if(max < 0) {
						std::cout << "mate " <<  -abs(max - BLACK_WIN) / 2 - 1;
					} else {
						std::cout << "mate " <<  abs(max - WHITE_WIN) / 2 + 1;
					}

					if(basis) {
						std::cout << "\nbestmove " << local_move.getMoveString();
						gameHash.push_back(getHash(game_board));
					}
				}
				std::cout << "\n";
			}

			return max;
		}
	}



	if(num_moves == 0) {
		if(inCheck(b, WHITE)) {
			return BLACK_WIN + real_depth;
		} else {
			return 0;
		}
	}

	if(depth == 0 && num_moves > 0) {
		bestmove = local_move;
		bestMove = bestmove;
		std::cout << "info pv " << local_move.getMoveString();
		std::cout << " nodes " << movesCounter;
		if(basis || rule == FIXED_TIME) {
			std::cout << " score ";
			if(max > BLACK_WIN + 10000 && max < WHITE_WIN - 10000) {
				std::cout << "cp " << (int) (max / PAWN_EV * 100);
			} else if(max < 0) {
				std::cout << "mate " <<  -abs(max - BLACK_WIN) / 2 - 1;
			} else {
				std::cout << "mate " <<  abs(max - WHITE_WIN) / 2 + 1;
			}

			if(basis) {
				std::cout << "\nbestmove " << local_move.getMoveString();
				gameHash.push_back(getHash(game_board));
			}
		}
		std::cout << "\n";
	}

	printVariant();

	if(hashEnable) {
		if(!boardHash[board_hash & hash_cutter].enable) {
			boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, max, REAL_EV);
		} else {
			if(boardHash[board_hash & hash_cutter].depth <= max_depth - real_depth) {
				boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, max, REAL_EV);
			}
		}
	}

	return max;
}

double Game::minimax_black(Board b, double alpha, double beta, int depth, int real_depth, std::vector<uint64_t> hash, bool basis, std::vector<Move>pv, bool usedNullMove, int rule) {
	if(rule == FIXED_TIME && timer.getTime() >= time) {
		return 0;
	}

	++movesCounter;

	if(depth >= max_depth) {
		return evalute(b);
		//	return quies(b, alpha, beta);
	}

	uint64_t pos_hash = getHash(b);
	uint64_t board_hash = getColorHash(b, pos_hash);

	if(depth > 0) {
	int third_repeat = 1;
		for(unsigned int i = 0; i < hash.size(); ++i) {
			if(hash[i] == pos_hash) {
				++third_repeat;
			}

			if(third_repeat >= 3 || b.move_rule_num >= 50) {
				return 0;
			}
		}
	}

	int num_moves = 0;

	double min = WHITE_WIN;

	//bool null_mv = nullMoveEnable;

	bool tmp_shah;
	std::vector<Move>moves = generatePositionMoves(b, tmp_shah, true, real_depth);

	Move local_move;

	if(moves.size() > 0) {
		local_move = moves[0];
	}

	for(unsigned int i = 0; i < moves.size(); ++i) {
		if(rule == FIXED_TIME) {
			if(timer.getTime() >= time) {
				return 0;
			}
		}
		Board tmp_brd = b;
		tmp_brd.move(moves[i]);

		if(inCheck(tmp_brd, BLACK)) {
			continue;
		}

		if(num_moves == 0) {
			local_move = moves[i];
		}

		++num_moves;

		double tmp;

		pv.push_back(moves[i]);
		hash.push_back(pos_hash);
		if(moves[i].fromHash && hashEnable) {
			if(boardHash[board_hash & hash_cutter].type_mv == ALPHA_CUT_EV) {
				beta = boardHash[board_hash & hash_cutter].evalute;
			}
		}

		/*if(num_moves <= 3 || inCheck(tmp_brd, WHITE) || depth < 3) {
				tmp = minimax_white(tmp_brd, alpha, beta, depth + 1, max_depth, real_depth + 1, hash, basis, pv, true);
		} else {
			tmp = minimax_white(tmp_brd, alpha-1, beta, depth + 2, max_depth, real_depth + 2, hash, basis, pv, true);
			if(tmp < beta) {
				//tmp_brd = b;
				tmp = minimax_white(tmp_brd, alpha, beta, depth + 1, max_depth, real_depth + 1, hash, basis, pv, true);
			}
		}*/

		if(num_moves == 1) {
				tmp = minimax_white(tmp_brd, alpha, beta, depth + 1, real_depth + 1, hash, basis, pv, true, rule);
		} else {
			tmp = minimax_white(tmp_brd, beta - 1, beta, depth + 1, real_depth + 1, hash, basis, pv, true, rule);

			if(tmp > alpha && tmp < beta) {
				tmp = minimax_white(tmp_brd, alpha, beta, depth + 1, real_depth + 1, hash, basis, pv, true, rule);
			}
		}

		//tmp = minimax_white(tmp_brd, alpha, beta, depth + 1, real_depth + 1, hash, basis, pv, true, rule);
		pv.pop_back();

		if(depth == 0) {
			std::cout << "info score cp " << -(int) (min / PAWN_EV * 100) << " depth " << max_depth << "\n";
			std::cout << "info currmove " << moves[i].getMoveString()  << " currmovenumber " << num_moves << "\n";
		}

		if(tmp < min) {
			min = tmp;
			local_move = moves[i];
			if(pv_tmp.size() < (unsigned int) (depth + 1)) {
				pv_tmp.resize(depth + 1);
			}
			pv_tmp[depth] = local_move;
			if(depth == 0) {
				pv_best = pv_tmp;
			}
		}

		if(tmp < beta) {
			beta = tmp;
			if(b.board[moves[i].toY][moves[i].toX] == 0) {
				blackKiller[real_depth] = Killer(local_move);
			}
		}

		if(min <= alpha) {
			if(b.board[moves[i].toY][moves[i].toX] == 0) {
				blackHistorySort[moves[i].fromY][moves[i].fromX][moves[i].toY][moves[i].toX] += pow(max_depth - real_depth, max_depth - real_depth);
			}

			if(hashEnable) {
				if(!boardHash[board_hash & hash_cutter].enable) {
					boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, min, ALPHA_CUT_EV);
				} else {
					if(boardHash[board_hash & hash_cutter].depth <= max_depth - real_depth) {
						boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, min, ALPHA_CUT_EV);
					}
				}
			}

			if(depth == 0) {
				bestmove = local_move;
				bestMove = bestmove;
				std::cout << "info pv " << local_move.getMoveString();
				std::cout << " nodes " << movesCounter;
				if(basis || rule == FIXED_TIME) {
					std::cout << " score ";
					if(min > BLACK_WIN + 10000 && min < WHITE_WIN - 10000) {
						std::cout << "cp " << -(int) (min / PAWN_EV * 100);
					} else if(min < 0) {
						std::cout << "mate " <<  abs(min - BLACK_WIN) / 2 + 1;
					} else {
						std::cout << "mate " <<  -abs(min - WHITE_WIN) / 2 - 1;
					}
					if(basis) {
						std::cout << "\nbestmove " << local_move.getMoveString();
						gameHash.push_back(getHash(game_board));
					}
				}
				std::cout << "\n";
			}

			return min;
		}
	}


	if(num_moves == 0) {
		if(inCheck(b, BLACK)) {
			return WHITE_WIN - real_depth;
		} else {
			return 0;
		}
	}

	if(depth == 0 && num_moves > 0) {
		bestmove = local_move;
		bestMove = bestmove;
		std::cout << "info pv " << local_move.getMoveString();
		std::cout << " nodes " << movesCounter;
		if(basis || rule == FIXED_TIME) {
			std::cout << " score ";
			if(min > BLACK_WIN + 10000 && min < WHITE_WIN - 10000) {
				std::cout << "cp " << -(int) (min / PAWN_EV * 100);
			} else if(min < 0) {
				std::cout << "mate " <<  abs(min - BLACK_WIN) / 2 + 1;
			} else {
				std::cout << "mate " <<  -abs(min - WHITE_WIN) / 2 - 1;
			}

			if(basis) {
				std::cout << "\nbestmove " << local_move.getMoveString();
				gameHash.push_back(getHash(game_board));
			}
		}
		std::cout << "\n";
	}

	if(hashEnable) {
		if(!boardHash[board_hash & hash_cutter].enable) {
			boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, min, REAL_EV);
		} else {
			if(boardHash[board_hash & hash_cutter].depth <= max_depth - real_depth) {
				boardHash[board_hash & hash_cutter] = Hash(board_hash, local_move, max_depth - real_depth, min, REAL_EV);
			}
		}
	}

	return min;
}

double Game::quies(Board b, double alpha, double beta, int rule) {
	if(rule == FIXED_TIME && timer.getTime() >= time) {
		return 0;
	}

	++movesCounter;
	double val = evalute(b);

	if(val > beta) {
		return beta;
	}

	if(alpha < val) {
		alpha = val;
	}

	bool tmp_shah = false;
	std::vector<Move>moves = generatePositionMoves(b, tmp_shah, true, max_depth);

	for(unsigned int i = 0; i < moves.size() && alpha < beta; ++i) {
		if(rule == FIXED_TIME && timer.getTime() >= time) {
			return 0;
		}

		if(b.board[moves[i].toY][moves[i].toX] == 0) {
			continue;
		}

		Board tmp_brd = b;
		tmp_brd.move(moves[i]);

		if(b.isWhiteMove()) {
			if(inCheck(tmp_brd, WHITE)) {
				continue;
			}
		} else {
			if(inCheck(tmp_brd, BLACK)) {
				continue;
			}
		}

		val = -quies(tmp_brd, -beta, -alpha, rule);

		if(val >= beta) {
			return val;
		}

		if(val > alpha) {
			alpha = val;
		}
	}

	return alpha;
}
