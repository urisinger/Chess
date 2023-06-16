#pragma once
#include "BitBoard.h"


//memory is super unsafe(never fixing this)
class ChessEngine {
public:
	ChessEngine(Board* board) { curBoard = board; count = 0; }

	int Minimax(int depth, const Board& board, int alpha, int beta, bool maximizingPlayer);
	Move BestMove();
	int quiescence(const Board& board, int alpha, int beta, bool maximizingPlayer, int depth);
	void setBoard(Board* board) { curBoard = board; }

	int maxDepth;
	int count;
private:
	Move bestMove;

	Board* curBoard;

	int killer_moves[2][100];

	int history_moves[12][64];

};