#include "ChessEngine.h"
#include <algorithm>



struct ScoredMove {
    int score;
    Move move;
};

namespace ChessEngine {

    int Minimax(int depth,const Board& board, int alpha, int beta, bool maximizingPlayer,Move* bestMoves) {
        if (depth == 0) {
            // Evaluate the current board position and return the evaluation score
            return (maximizingPlayer ? 1 : -1) * (board.currentPlayer == WHITE ? 1 : -1) * board.eval();
        }

        auto moves = board.GenerateLegalMoves(board.currentPlayer);
        std::sort(moves.moves, moves.moves + moves.count, [](const Move& a, const Move& b) {

            return (a.getFlags()) > (b.getFlags());
            });

        if (moves.count == 0) {
            // Handle the case where no legal moves are available
            return board.isKingAttacked(board.currentPlayer) ? (maximizingPlayer ? std::numeric_limits<int>::min()+1 : std::numeric_limits<int>::max()-1) : 0;
        }

        if (maximizingPlayer) {
            int maxScore = std::numeric_limits<int>::min();
            for (int i = 0; i < moves.count; i++) {
                Board newboard = board;
                newboard.movePiece(moves.moves[i]);


                int currentScore = Minimax(depth - 1, newboard, alpha, beta, false, bestMoves);

                if (maxScore <= currentScore) {
                    bestMoves[depth-1] = moves.moves[i];
                }
                maxScore = std::max(maxScore, currentScore);
                alpha = std::max(alpha, maxScore);

                if (alpha >= beta) {
                    // Beta cutoff
                    break;
                }
            }
            return maxScore;
        }
        else {
            int minScore = std::numeric_limits<int>::max();
            for (int i = 0; i < moves.count; i++) {
                Board newboard = board;
                newboard.movePiece(moves.moves[i]);

                int currentScore = Minimax(depth - 1, newboard, alpha, beta, true, bestMoves);

                if (minScore >= currentScore) {
                    bestMoves[depth-1] = moves.moves[i];
                }

                minScore = std::min(minScore, currentScore);
                beta = std::min(beta, minScore);

                if (beta <= alpha) {
                    // Alpha cutoff
                    break;
                }
            }
            return minScore;
        }
    }

    Move* BestMove(int depth, Board& board) {
        auto moves = board.GenerateLegalMoves(board.currentPlayer);
        if (moves.count == 0) {
            // Handle the case where no legal moves are available
            return nullptr; // Return an empty move
        }



        int alpha = std::numeric_limits<int>::min();
        int beta = std::numeric_limits<int>::max();
        Move *bestMoves = new Move[depth];

        int currentScore = Minimax(depth, board, alpha, beta, true, bestMoves);

        std::cout << currentScore << std::endl;
        return bestMoves;
    }

}   