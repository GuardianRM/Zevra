#include "game.hpp"

int64_t Game::negamax(BitBoard & b, int64_t alpha, int64_t beta, int depth, int real_depth, int rule, bool inNullMove, bool cut) {
	++nodesCounter;

	uint64_t hash = b.getColorHash();
	Hash* currentHash = &boardHash[hash & hash_cutter];

	if(!b.currentState.hash_enable && real_depth > 0) {
		return 0;
	}

	int64_t oldAlpha = alpha;

	if((/*currentHash->flag != EMPTY &&*/ currentHash->key == hash)) {
		if(real_depth > 0 && currentHash->depth >= depth) {
			int upperbound = currentHash->upperbound;
			int lowerbound = currentHash->lowerbound;

			//7k/3P2r1/p6p/P1p2p2/3b1PnP/1Q3R1K/1P2q3/7R b - - 0 1


			if(upperbound > WHITE_WIN - 100) {
				upperbound -= real_depth;
			} else if(upperbound < -WHITE_WIN + 100) {
				upperbound += real_depth;
			}

			if(lowerbound > WHITE_WIN - 100) {
				lowerbound -= real_depth;
			} else if(lowerbound < -WHITE_WIN + 100) {
				lowerbound += real_depth;
			}

			if(lowerbound >= beta) {
				return lowerbound;
			}

			if(upperbound <= alpha) {
				return upperbound;
			}

			alpha = std::max((int)alpha, lowerbound);
			beta = std::min((int)beta, upperbound);

			if(alpha >= beta) {
				return beta;
			}
		}
	}

	if(depth <= 0 || real_depth >= 100) {
		return quies(b, alpha, beta, rule, real_depth);
	}

	if(depth > 6) {
		if(is_input_available()) {
			std::string input_str;
			std::getline(std::cin, input_str);
			if(input_str == "stop") {
				stopped = true;
			} else if(input_str == "isready") {
				std::cout << "readyok" << std::endl;
			}
		}
	}

	if(real_depth) {
		max_real_depth = std::max(max_real_depth, real_depth);
	} else {
		max_real_depth = 0;
	}

	bool extended = false;

	int64_t eval = -WHITE_WIN;

	int nextDepth = depth - 1;
	int extensions = 0;

	bool checkMateNode = (std::abs(beta) >= WHITE_WIN - 100);

	if(depth > 2) {
		if((rule == FIXED_TIME && timer.getTime() >= time) || stopped) {
			return 0;
		}
	}

	uint8_t color, enemyColor;
	if(b.currentState.whiteMove) {
		color = WHITE;
		enemyColor = BLACK;
	} else {
		color = BLACK;
		enemyColor = WHITE;
	}

	int tmp;


	bool inCheck;
	inCheck = b.inCheck(color);

	bool onPV = (beta - alpha) > 1;

	int opposiing_pieces = (color == WHITE ? b.popcount64(b.currentState.black_bit_mask) : b.popcount64(b.currentState.white_bit_mask));

	if(!extended && !inCheck &&  !inNullMove && depth < 10 && !onPV && opposiing_pieces > 3) { //Razoring
		if(b.getEvaluate() - RAZOR_MARGIN[depth] >= beta) {
			//rn1qr1k1/1p2bppp/p3p3/3pP3/P2P1B2/2RB1Q1P/1P3PP1/R5K1 w - - 0 1
			return beta;
		}
	}

	int num_moves = 0;

	b.bitBoardMoveGenerator(moveArray[real_depth], stress);
	sortAttacks(moveArray[real_depth]);
	sortMoves(moveArray[real_depth], real_depth);

	if(cut) { //Multi-Cut
		if (depth >= 4 && real_depth > 0 && !inCheck && !inNullMove && !extensions && beta - alpha <= 1 && !b.currentState.attacked) {
			int c = 0;
			for (int i = 0; i < std::min((int)moveArray[real_depth].count, 4); ++i) {
				b.move(moveArray[real_depth].moveArray[i]);
				if(b.inCheck(color)) {
					b.goBack();
					continue;
				}
				
				tmp = -negamax(b, -(alpha + 1), -alpha, nextDepth - 4, real_depth + 1, rule, inNullMove, false);
				
				b.goBack();
				if (tmp >= beta) {
					if (++c == 3) {
						return beta;
					}
				}
			}
		}
	}

	int R = 2 + depth / 6;
	
	if(!inNullMove && !extended && !inCheck && !onPV && depth > R && (b.popcount64(b.currentState.white_bit_mask | b.currentState.black_bit_mask) > 6) && real_depth > 0) {
		b.makeNullMove();
		double value = -negamax(b, -beta, -beta + 1, depth - R - 1, real_depth + 1, rule, true, true);
		if(value >= beta) {
			b.unMakeNullMove();
			return value;
		}
		b.unMakeNullMove();
	}

	BitMove local_move;

	if(moveArray[real_depth].count > 0) {
		local_move = moveArray[real_depth].moveArray[0];
	}

	int low_moves_count = 0;

	for(unsigned int i = 0; i < moveArray[real_depth].count; ++i) {
		b.move(moveArray[real_depth].moveArray[i]);

		if(game_board.inCheck(color)) {
			b.goBack();
			continue;
		}

		if(num_moves == 0) {
			local_move = moveArray[real_depth].moveArray[i];
		}

		++num_moves;

		if(real_depth == 0 && depth >= 12) {
			if((rule != FIXED_TIME || timer.getTime() < time) && !stopped) {
				std::cout << "info currmove " << moveArray[real_depth].moveArray[i].getMoveString() << " currmovenumber " << num_moves << " time " << (int64_t)((clock() - start_timer) / (CLOCKS_PER_SEC / 1000)) << " nodes " << nodesCounter << " nps " << (int64_t)(nodesCounter / ((clock() - start_timer) / CLOCKS_PER_SEC)) << std::endl;
			}	
		}

		extensions = 0;
		if(b.inCheck(enemyColor)) {
			++extensions;
		}

		nextDepth = depth - 1;
		nextDepth += extensions;
		double reduction = 0;
		//r1b1k2N/ppp1q1pp/2n2n2/8/Q7/2N1b3/PPP2PPP/R3KB1R b KQq - 0 11

		if(!b.inCheck(enemyColor) && !extensions && !inNullMove && !moveArray[real_depth].moveArray[i].isAttack && !onPV && !inCheck) {
			++low_moves_count;

			if(low_moves_count > 3) {
				reduction = 1 + low_moves_count / 6;
				nextDepth -= reduction;
			}

			if(nextDepth <= 2) {
				if(-b.getEvaluate() + PAWN_EV.mg / 2 <= alpha) {
					++nodesCounter;
					b.goBack();
					continue;
				}
			}
		}

		if(num_moves == 1) {
			tmp = -negamax(b, -beta, -alpha, nextDepth, real_depth + 1, rule, inNullMove, true);	
		} else {
			tmp = -negamax(b, -(alpha + 1), -alpha, nextDepth, real_depth + 1, rule, inNullMove, true);
			
			if(reduction > 0 && tmp > alpha) {
				nextDepth += reduction;
				tmp = -negamax(b, -(alpha + 1), -alpha, nextDepth, real_depth + 1, rule, inNullMove, true);
			}

			if(tmp > alpha && tmp < beta) {
				tmp = -negamax(b, -beta, -alpha, nextDepth, real_depth + 1, rule, inNullMove, true);
			}
		}

		b.goBack();


		eval = std::max((int)eval, tmp);

		if(eval > alpha) {
			alpha = eval;
			local_move = moveArray[real_depth].moveArray[i];
			//recordHash(depth, tmp, tmp<beta?EXACT:BETA, hash, moveArray[real_depth].moveArray[i], real_depth);
		}
		
		if(alpha >= beta) {
			if(!local_move.isAttack) {
				whiteHistorySort[local_move.fromY][local_move.fromX][local_move.toY][local_move.toX] += std::pow(depth, 2);

				if(color == WHITE) {
					if(whiteKiller[real_depth].enable) {
						whiteSecondKiller[real_depth] = Killer(whiteKiller[real_depth].move);
					}
					whiteKiller[real_depth] = Killer(local_move);
				} else {
					if(blackKiller[real_depth].enable) {
						blackSecondKiller[real_depth] = Killer(blackKiller[real_depth].move);
					}
					blackKiller[real_depth] = Killer(local_move);
				}
			}
			return beta;
		}
	}

	if(num_moves == 0) {
		if(game_board.inCheck(color)) {
			return BLACK_WIN + real_depth;
		} else {
			return 0;
		}
	}

	if(eval <= oldAlpha) {
		recordHash(depth, eval, UPPERBOUND, hash, local_move, real_depth);
	} else if(eval > oldAlpha && eval < beta) {
		recordHash(depth, eval, EXACT, hash, local_move, real_depth);
	} else {
		recordHash(depth, eval, LOWERBOUND, hash, local_move, real_depth);
	}

	if(real_depth == 0) {
		if((rule == FIXED_TIME && timer.getTime() >= time) || stopped) {
			return 0;
		}

		bestMove = local_move;
		bestScore = alpha;
	}

	if(real_depth == 0) {
		if(num_moves >= 0) {
			std::cout << "info depth " << max_depth << " time " << (int64_t)((clock() - start_timer) / (CLOCKS_PER_SEC / 1000)) << " nodes " << nodesCounter << " nps " << (int64_t)(nodesCounter / ((clock() - start_timer) / CLOCKS_PER_SEC)) << " hashfull " << (int)(hash_filled / max_hash_filled * 1000) << " seldepth " << max_real_depth;
			std::cout << " ";
			printScore(eval);
			std::cout << " pv ";
			printPV(depth);
		} else {
			std::cout << std::endl;
		}
	}

	return alpha;
}

uint64_t Game::perft(int depth) {
	uint64_t res = 0;
	if(depth == 0) {
		++combinations;

		return 1;
	}

	game_board.bitBoardMoveGenerator(moveArray[depth], stress);

	for(unsigned int i = 0; i < moveArray[depth].count; ++i) {
		game_board.move(moveArray[depth].moveArray[i]);
		if(game_board.currentState.whiteMove) {
			if(game_board.inCheck(BLACK)) {
				game_board.goBack();
				continue;
			}
		} else {
			if(game_board.inCheck(WHITE)) {
				game_board.goBack();
				continue;
			}
		}

		res += perft(depth - 1);
		game_board.goBack();
	}

	return res;
}

int64_t Game::quies(BitBoard & b, int64_t alpha, int64_t beta, int rule, int real_depth) {
	int64_t val = b.getEvaluate();

	if (val < alpha - QUEEN_EV.mg) {
   		return alpha;
	}

	if(val >= beta) {
		return val;
	}

	if(alpha < val) {
		alpha = val;
	}

	b.bitBoardAttackMoveGenerator(moveArray[real_depth], stress);
	sortAttacks(moveArray[real_depth]);

	for(unsigned int i = 0; i < moveArray[real_depth].count; ++i) {
		++nodesCounter;
		b.move(moveArray[real_depth].moveArray[i]);

		if(!b.currentState.whiteMove) {
			if(game_board.inCheck(WHITE)) {
				b.goBack();
				continue;
			} 
		} else {
			if(game_board.inCheck(BLACK)) {
				b.goBack();
				continue;
			}
		}

		val = -quies(b, -beta, -alpha, rule, real_depth + 1);
		b.goBack();

		if(val >= beta) {
			return beta;
		}

		if(val > alpha) {
			alpha = val;
		}
	}

	return alpha;
}

bool Game::recordHash(int depth, int score, int flag, uint64_t key, BitMove move, int real_depth) {
	if(!game_board.currentState.hash_enable) {
		return false;
	}

	Hash* hash = &boardHash[key & hash_cutter];

	/*if(flag == ALPHA && (hash->flag == EXACT || hash->flag == BETA)) {
		return false;
	}*/

	if((hash->flag != EMPTY) && hash->depth > depth && hash->age == hashAge) {
		return false;
	}

	if(score > WHITE_WIN - 100) {
		score += real_depth;
	} else if(score < -WHITE_WIN + 100) {
		score -= real_depth;
	}

	if(hash->flag == EMPTY) {
		++hash_filled;
	}

	hash->depth = depth;

	if(flag == LOWERBOUND) {
		hash->lowerbound = score;
	} else if(flag == UPPERBOUND) {
		hash->upperbound = score;
	} else {
		hash->lowerbound = score;
		hash->upperbound = score;
	}

	hash->flag = flag;
	hash->key = key;
	hash->age = hashAge;

	hash->setMove(move);

	return true;
}

std::vector<BitMove> Game::extractPV(int depth) {
	int k;
	std::vector<BitMove> result;
	MoveArray moves;
	bool stopped;
	int stack_count = 0;
	for(k = 0; k < depth + 10; ++k) {
		stopped = true;
		uint64_t hash = game_board.getColorHash();
		Hash* currentHash = &boardHash[hash & hash_cutter];

		if(currentHash->flag != EMPTY) {
			game_board.bitBoardMoveGenerator(moves, stress);

			bool enable;
			BitMove mv = game_board.getMove(currentHash->fromY, currentHash->fromX, currentHash->toY, currentHash->toX, currentHash->replaced, currentHash->replacedFigure, enable);// currentHash->getMove();

			for(int i = 0; i < moves.count; ++i) {
				if(moves.moveArray[i].equal(mv)) {
					game_board.move(mv);
					++stack_count;
					if(game_board.currentState.whiteMove) {
						if(game_board.inCheck(BLACK)) {
							stopped = true;
						} else {
							stopped = false;
						}
					} else {
						if(game_board.inCheck(WHITE)) {
							stopped = true;
						} else {
							stopped = false;
						}
					}
				}
			}

			if(stopped || !enable) {
				break;
			}

			result.emplace_back(mv);
		} else {
			break;
		}
	}

	for(int i = 0; i < stack_count; ++i) {
		game_board.goBack();
	}

	return result;
}


bool Game::testMovePossible(BitMove move) {
	MoveArray moves;
	game_board.bitBoardMoveGenerator(moves, stress);

	for(unsigned int i = 0; i < moves.count; ++i) {
		if(move.equal(moves.moveArray[i])) {
			return true;
		}
	}

	return false;
}

void Game::printPV(int depth) {
	std::vector<BitMove> main_pv = extractPV(depth);
	for(unsigned int i = 0; i < main_pv.size(); ++i) {
		std::cout << main_pv[i].getMoveString() << " ";
	}
	std::cout << std::endl;
}