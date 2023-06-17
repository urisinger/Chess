#pragma once
#include "BitBoard.h"
#define MIN_SCORE -100000
#define MAX_SCORE 100000

#define max_ply 100

//memory is super unsafe(never fixing this)
class ChessEngine {
public:
	ChessEngine(Board* board) { curBoard = board; count = 0; ply = 0; }

	int Minimax(int depth, const Board& board, int alpha, int beta);
	Move BestMove(double maxTime);
	int quiescence(const Board& board, int alpha, int beta);
	void setBoard(Board* board) { curBoard = board; }

	static int count;

	static Move killer_moves[2][max_ply];

	static int pv_length[max_ply];
	static Move pv_table[max_ply][max_ply];

	static int history_moves[12][64];

	static int ply;
private:

	static const int reductionLimits;
	static const int fullDepthMoves;
	static Board* curBoard;
};