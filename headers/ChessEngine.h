#pragma once
#include "BitBoard.h"


//memory is super unsafe(never fixing this)
class ChessEngine {
public:
	ChessEngine(Board* board) { curBoard = board; count = 0; }

	int ChessEngine::Minimax(int depth, const Board& board, int alpha, int beta, bool maximizingPlayer);
	Move BestMove();
	int ChessEngine::quiescence(const Board& board, int alpha, int beta, bool maximizingPlayer);
	void setBoard(Board* board) { curBoard = board; }

	int maxDepth;
	int count;
private:
	Move bestMove;

	Board* curBoard;

};