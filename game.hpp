#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <ctime>
#include <string>
#include <cstdlib>
#include <random>
#include <thread>
#include <cstdio>
#include <ctime>
#include <future>
#include <thread>
#include <iomanip>
#include <algorithm>
#include <stack>
#include <memory.h>
#include "bitboard.hpp"
#include "constants.hpp"
#include "killer.hpp"
#include "hash.hpp"
#include "timer.hpp"
#include "movearray.hpp"
#include "option.hpp"

class Game {
private:
	BitBoard game_board;
	unsigned long long nodesCounter = 0;

	int64_t max_depth;
	bool stopped;

	int hash_decrement = 0;
	int hash_width;
	uint64_t hash_cutter;

	void initEngine();
	bool uciHandler(std::string str);
	void goFixedDepth();
	void goFixedTime(int64_t time, bool tournamentTimeControll);
	void goTournament();
	void clearCash();
	bool move_comparator(BitMove& m1, BitMove&m2);
	void initializeArrays();
	void setHashSize(int mb_size);

  	std::vector<BitMove>pv;
	std::vector<size_t> count_moves;

	double whiteHistorySort[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
	double blackHistorySort[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];

	bool hasBestMove = false;

	size_t stress;
	BitMove bestMove;
	int64_t bestScore;
	int max_real_depth;
	uint8_t hashAge = 0;
	double hash_filled, max_hash_filled;
	int halfMoveNumber = 0;

	//фиксированное время

	Timer timer;
	int64_t time;
	double start_timer, end_timer;

	//турнирный режим

	int64_t wtime, btime, winc, binc;
	uint64_t movestogo;
	bool movestogoEnable;
	uint64_t combinations;

	Option option;

public:
	Game();
	int64_t negamax(BitBoard & b, int64_t alpha, int64_t beta, int depth, int real_depth, int rule, bool inNullMove, bool cut);

	uint64_t perft(int depth);
	bool insufficientMaterial(std::vector<uint64_t>figureMask);
  	int64_t quies(BitBoard & b, int64_t alpha, int64_t beta, int rule, int real_depth);
	int startGame();
	void printScore(double score);
	std::vector<std::string> getStringArray(std::string str);
	bool move(std::string mv);
	void printBitBitBoard(uint64_t bit_BitBoard);
	void idPrint();

	std::vector<uint64_t>gameHash;
	std::vector<Hash>boardHash;

	std::vector<Killer>whiteKiller;
	std::vector<Killer>blackKiller;
	std::vector<Killer>whiteSecondKiller;
	std::vector<Killer>blackSecondKiller;
	
	std::vector<Killer>whiteMateKiller;
	std::vector<Killer>blackMateKiller;

	std::vector<MoveArray> moveArray;

	void sortAttacks(MoveArray& moves);

	double evalute_cells_size[BOARD_SIZE][BOARD_SIZE][BOARD_SIZE][BOARD_SIZE];
	void cleanWhiteHistory();
	void cleanBlackHistory();
	void flattenHistory();
	void printPV(int depth);

	void sortMoves(MoveArray& result, int depth);
	bool recordHash(int depth, int score, int flag, uint64_t key, BitMove move, int real_depth);

	std::vector<BitMove> extractPV(int depth);

	bool testMovePossible(BitMove move);
};

#endif
