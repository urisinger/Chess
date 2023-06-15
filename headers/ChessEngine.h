#pragma once
#include "BitBoard.h"


//memory is super unsafe(never fixing this)
class ChessEngine {
public:
	ChessEngine(Board* board) { curBoard = board; }

	int ChessEngine::Minimax(int depth, const Board& board, int alpha, int beta, bool maximizingPlayer);
	Move BestMove();
	int ChessEngine::quiescence(const Board& board, int alpha, int beta, bool maximizingPlayer) const;
		void setBoard(Board* board) { curBoard = board; }

	int maxDepth;
private:
	Move bestMove;

	Board* curBoard;

};