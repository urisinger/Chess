#include "ChessEngine.h"
#include <algorithm>

#define MIN_SCORE -100000
#define MAX_SCORE 100000

//this is needed becuase the value of the piece enum is used for rendering too
constexpr static int translator[6] =
{
    4,
    5,
    3,
    1,
    2,
    0
};



int compareMoves(const void* a, const void* b) {
    const Move* moveA = static_cast<const Move*>(a);
    const Move* moveB = static_cast<const Move*>(b);

    // Compare based on move flags
    if (moveA->getFlags() < moveB->getFlags()) {
        return 1;
    }
    else if (moveA->getFlags() > moveB->getFlags()) {
        return -1;
    }
    else if (moveA->getCapturedPiece() != EMPTY && moveB->getCapturedPiece() != EMPTY) {
        // Compare capture moves
        //translator is just a shortcut i use cuz  i use the vaalue of the piece enum for rendering too, just treat it as the pieces ordered by value
        int firstone = (translator[moveA->getPiece() - 1]) + (5 - translator[moveA->getCapturedPiece() - 1] + 1) * 100;
        int secondone = (translator[moveB->getPiece() - 1]) + (5 - translator[moveB->getCapturedPiece() - 1] + 1) * 100;

        if (firstone > secondone) {
            return 1;
        }
        else if (firstone < secondone) {
            return -1;
        }
        else {
            return 0;
        }
    }
    else {
        // For non-capture moves, no comparison needed
        return 0;
    }
}


int ChessEngine::quiescence(const Board& board, int alpha, int beta, bool maximizingPlayer,int depth) {
    int standPat = (maximizingPlayer*2-1)*(board.currentPlayer*2-1)*board.eval(); // Evaluate the current position without considering captures or promotions

    if (depth == 0) {
        return standPat;
    }
    if (maximizingPlayer) {
        if (standPat >= beta) {
            return beta; // Return beta if the standPat score is already greater than or equal to beta for the maximizing player
        }
        alpha = std::max(alpha, standPat); // Update alpha with the maximum value between alpha and standPat for the maximizing player
    }
    else {
        if (standPat <= alpha) {
            return alpha; // Return alpha if the standPat score is already less than or equal to alpha for the minimizing player
        }
        beta = std::min(beta, standPat); // Update beta with the minimum value between beta and standPat for the minimizing player
    }

    auto captures = board.GenerateCaptureMoves(board.currentPlayer); // Generate all capture moves

    qsort(captures.moves, captures.count, sizeof(Move), compareMoves);

    for (int i = 0; i < captures.count; i++) {
        count++;
        Board newboard = board;
        newboard.movePiece(captures.moves[i]);

        int score = quiescence(newboard, alpha, beta, !maximizingPlayer, depth-1); // Recursively evaluate the capture move with the opposite maximizingPlayer flag

        if (maximizingPlayer) {
            if (score >= beta)
                return beta;
            if (score > alpha)
                alpha = score;
        }
        else {
            if (score <= alpha)
                return alpha;
            if (score < beta)
                beta = score;
        }
    }

    if (maximizingPlayer) {
        return alpha;
    }
    else {
        return beta;
    }
}




int ChessEngine::Minimax(int depth, const Board& board, int alpha, int beta, bool maximizingPlayer) {
    count++;

    if (depth == 0) {
        // Evaluate the current board position and return the evaluation score
        return quiescence(board, alpha, beta,maximizingPlayer,100);
    }

    auto moves = board.GenerateLegalMoves(board.currentPlayer);

    qsort(moves.moves, moves.count, sizeof(Move), compareMoves);

    if (moves.count == 0) {
        // Handle the case where no legal moves are available
        count++;
        return board.isKingAttacked(board.currentPlayer) ? (maximizingPlayer ? MIN_SCORE - depth : MAX_SCORE + depth) : 0;
    }

    if (maximizingPlayer) {
        int maxScore = std::numeric_limits<int>::min();
        for (int i = 0; i < moves.count; i++) {
            Board newboard = board;
            newboard.movePiece(moves.moves[i]);


            int currentScore = Minimax(depth - 1, newboard, alpha, beta, false);

            if (maxScore < currentScore) {
                if (maxDepth - depth == 0) {
                    bestMove = moves.moves[i];
                    std::cout << currentScore << ": " << bestMove.to_str() << std::endl;
                }
                maxScore = currentScore;
            }

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

            int currentScore = Minimax(depth - 1, newboard, alpha, beta, true);

            if (minScore >= currentScore) {
                if (maxDepth - depth == 0) {
                    bestMove = moves.moves[i];
                    std::cout << currentScore << ": " << bestMove.to_str() << std::endl;
                }
                minScore = currentScore;
            }

            beta = std::min(beta, minScore);

            if (beta <= alpha) {
                // Alpha cutoff
                break;
            }
        }
        return minScore;
    }
    return alpha;
}


Move ChessEngine::BestMove() {
    auto moves = curBoard->GenerateLegalMoves(curBoard->currentPlayer);
    if (moves.count == 0) {
        // Handle the case where no legal moves are available
        Move empty;
        return empty; // Return an empty move
    }



    int alpha = MIN_SCORE;
    int beta = MAX_SCORE;

    count = 0;
    int currentScore = Minimax(maxDepth, *curBoard, alpha, beta,true);

    std::cout << "nodes searched: " << count << " score: " << currentScore << std::endl;
    return bestMove;
}