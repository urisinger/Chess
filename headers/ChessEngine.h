#pragma once
#include "BitBoard.h"

namespace ChessEngine {

	int Minimax(int depth, const Board& board, int alpha, int beta, bool maximizingPlayer, Move* bestMoves);
	Move* BestMove(int depth, Board& board);
};