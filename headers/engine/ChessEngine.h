#pragma once
#include "TT.h"



class ChessEngine {
public:
	ChessEngine() : quit(false), Table(HASH_SIZE), count(0), ply(0), repetitionIndex(0){ }

	Move BestMove(int maxDdepth, const Board& board);

	void RunPerftTest(int depth, const Board& board);


	uint64_t repetitionTable[max_repetition];
	int repetitionIndex;
	TTable Table;

	Move BestLine[max_ply];


	bool quit;
	bool stop;
	int startTime;

	double maxTime;
	int ply;
	int count;
private:


	Move killer_moves[2][max_ply];

	int pv_length[max_ply];
	Move pv_table[max_ply][max_ply];

	int history_moves[12][64];

	int score_move(Move move);
	inline bool compareMoves(Move moveA, Move moveB);
	void swapMoves(Move& moveA, Move& moveB);

	bool IsRepetition(uint64_t hash);

	int Perft(int depth, const Board& board);

	int NegMax(int depth, const Board& board, int alpha, int beta);
	int quiescence(const Board& board, int alpha, int beta);

	void communicate();
	void clearTables();
	const int reductionLimits = 3;
	const int fullDepthMoves = 4;
};



