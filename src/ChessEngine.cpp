#include "ChessEngine.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <conio.h>
#include <iostream>
#include <string>
#include <chrono>
#ifdef WIN32
#include <Windows.h>
#endif




int GetTimeMs() {
#ifdef WIN32
    return GetTickCount();
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
#endif
}
void ChessEngine::communicate() {
    if (GetTimeMs() - startTime >= maxTime) {
        stop = true;
    }
}



//this is needed becuase the value of the piece enum is used for rendering too
constexpr static int translator[6] =
{
    5,
    6,
    4,
    2,
    3,
    1
};




int ChessEngine::score_move(Move move) {


    if (move.getCapturedPiece() != EMPTY) {
        // Compare capture moves
        //translator is just a shortcut i use cuz  i use the vaalue of the piece enum for rendering too, just treat it as the pieces ordered by value
        return (6 - translator[move.getPiece() - 1]) + (translator[move.getCapturedPiece() - 1]) * 100 + 10000;
    }
    else {


        if (killer_moves[0][ply] == move) {
            return 9000;
        }

        if (killer_moves[1][ply] == move) {
            return 8000;
        }

        return history_moves[(move.getPiece() - 1) * (move.getColor() + 1)][move.getTo()];

    }


}




int ChessEngine::compareMoves(Move moveA, Move moveB, Move bestMove) {
    if (moveA == bestMove) {
        return -1;  // moveA is the best move, so it should come first
    }
    else if (moveB == bestMove) {
        return 1;  // moveB is the best move, so it should come first
    }
    else if (score_move(moveA) < score_move(moveB)) {
        return 1;
    }
    else if (score_move(moveA) > score_move(moveB)) {
        return -1;
    }
    else {
        return 0;
    }
}
void ChessEngine::swapMoves(Move& moveA, Move& moveB) {
    Move temp = moveA;
    moveA = moveB;
    moveB = temp;
}

int ChessEngine::partitionMoves(Move* moves, int low, int high, Move bestMove) {
    Move pivot = moves[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (compareMoves(moves[j], pivot, bestMove) < 0) {
            i++;
            swapMoves(moves[i], moves[j]);
        }
    }

    swapMoves(moves[i + 1], moves[high]);
    return i + 1;
}

void ChessEngine::quicksortMoves(Move* moves, int low, int high, Move bestMove) {
    if (low < high) {
        int pivotIndex = partitionMoves(moves, low, high, bestMove);
        quicksortMoves(moves, low, pivotIndex - 1, bestMove);
        quicksortMoves(moves, pivotIndex + 1, high, bestMove);
    }
}



void ChessEngine::sortMoves(LegalMoves& legalMoves, Move bestMove) {
    quicksortMoves(legalMoves.moves, 0, legalMoves.count - 1,bestMove);
}


int ChessEngine::quiescence(const Board& board, int alpha, int beta) {
    count++;

    int standPat = board.currentPlayer == WHITE ? board.eval() : -board.eval(); // Evaluate the current position without considering captures or promotions
    int movesSearched = 0;

    if (ply >= max_ply) {
        return standPat;
    }

    if (standPat >= beta) {
        return beta; // Return beta if the standPat score is already greater than or equal to beta for the maximizing player
    }



    alpha = max(alpha, standPat); // Update alpha with the maximum value between alpha and standPat for the maximizing player

    auto captures = board.GenerateCaptureMoves(board.currentPlayer); // Generate all capture moves

    sortMoves(captures, Move());

    for (int i = 0; i < captures.count; i++) {
        Board newboard = board;
        newboard.movePiece(captures.moves[i]);

        if ((newboard.isKingAttacked(board.currentPlayer))) {
            // Move doesn't result in own king being in check
            continue;

        }

        if (captures.moves[i].getFlags() == QUEEN_CASTLE || captures.moves[i].getFlags() == KING_CASTLE) {
            if (board.isKingAttacked(board.currentPlayer)) {
                continue;
            }

        }

        if (stop) {
            return STOPPED;
        }

        ply++;
        int score = -quiescence(newboard, -beta, -alpha); // Recursively evaluate the capture move with the opposite maximizingPlayer flag
        ply--;

     //   *bestMoveP = bestMove;

        movesSearched++;

        if (score > alpha) {
            alpha = score;
        }

        if (score >= beta) {
            return beta; // Perform a cutoff if alpha is greater than or equal to beta
        }
    }



    return alpha;
}



#define R 2

int ChessEngine::NegMax(int depth, const Board& board, int alpha, int beta) {
    pv_length[ply] = ply;

    if (count % 2048) {
        communicate();
    }


    int score;

    HashFlags hashFlag = HASH_ALPHA;
    int movesSearched = 0;

    Move bestMove;
    
    int pv_node = (beta - alpha > 1);

    score = Trasposition.ReadHash(board.hashKey, alpha, beta, &bestMove , depth,ply);
    if (score != NO_HASH_ENTRY && !pv_node)
    {
        return score;
    }

    bool in_check = board.isKingAttacked(board.currentPlayer);

    depth += in_check;
    if (depth == 0) {
        // Evaluate the current board position and return the evaluation score
        score = quiescence(board, alpha, beta);
        return score;
    }

    if (depth >= R + 1 && !in_check && ply) {
        Board nullBoard = board;

        nullBoard.currentPlayer = nullBoard.currentPlayer == WHITE ? BLACK : WHITE;

        if (nullBoard.enPassantSquare != -1) {
            nullBoard.hashKey ^= Masks::enPeasentKeys[nullBoard.enPassantSquare];
        }
        nullBoard.enPassantSquare = -1;

        nullBoard.hashKey ^= Masks::SideKey;

        ply += R + 1;
        int score = -NegMax(depth - 1 - R, nullBoard, -beta, -beta + 1);
        ply -= R + 1;

        if (score >= beta)
            return beta;
    }



    LegalMoves moves = board.GenerateLegalMoves(board.currentPlayer);

    sortMoves(moves,bestMove);


    for (int i = 0; i < moves.count; i++) {
        Board newboard = board;
        newboard.movePiece(moves.moves[i]);

        if ((newboard.isKingAttacked(board.currentPlayer))) {
            // Move doesn't result in own king being in check
            continue;
        }

        if (moves.moves[i].getFlags() == QUEEN_CASTLE || moves.moves[i].getFlags() == KING_CASTLE) {
            if (board.isKingAttacked(board.currentPlayer)) {
                continue;
            }

            // Get the starting and ending positions for the castle move
            int startPos = moves.moves[i].getFrom();
            int endPos = moves.moves[i].getTo();

            // Check the squares between the starting and ending positions
            int minPos = min(startPos, endPos);
            int maxPos = max(startPos, endPos);

            // Iterate over the squares between the starting and ending positions
            for (int square = minPos + 1; square < maxPos; square++) {
                if (board.isSqaureAttacked(board.currentPlayer, square)) {
                    // Square is attacked, so it is not safe to castle
                    continue;
                }
            }
        }

        

        if (stop) {
            return STOPPED;
        }

        if (movesSearched == 0) {
            ply++;
            score = -NegMax(depth - 1, newboard, -beta, -alpha);
            ply--;
           // *bestMoveP = bestMove;
        }
        else {
            if (movesSearched >= fullDepthMoves && depth >= reductionLimits && !in_check && moves.moves[i].getFlags() < 4) {
                ply += 2;
                score = -NegMax(depth - 2, newboard, -alpha - 1, -alpha);
                ply -= 2;

            }
            else
                score = alpha + 1;

            if (score > alpha) {
                ply++;
                score = -NegMax(depth - 1, newboard, -alpha - 1, -alpha);
                ply--;


                if (score > alpha && score < beta) {
                    ply++;
                    score = -NegMax(depth - 1, newboard, -beta, -alpha);
                    ply--;
                }
            }
        }


        movesSearched++;
        count++;



        if (score > alpha) {
            hashFlag = HASH_EXSACT;

            if (moves.moves[i].getCapturedPiece() == EMPTY) {
                history_moves[(moves.moves[i].getPiece() - 1) * (moves.moves[i].getColor() + 1)][moves.moves[i].getTo()] += depth;
            }
            pv_table[ply][ply] = moves.moves[i];

            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++) {
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            }

            pv_length[ply] = pv_length[ply + 1];

            alpha = score;
            bestMove = moves.moves[i];
        }

        if (score >= beta) {
            Trasposition.WriteHash(board.hashKey, score, depth,bestMove, HASH_BETA,ply);
            killer_moves[1][ply] = killer_moves[0][ply];
            killer_moves[0][ply] = moves.moves[i];
            return beta;
        }

    }

    if (movesSearched == 0) {
        // Handle the case where no legal moves are available
        score = in_check * (board.currentPlayer * 2 - 1) * (MATE_VALUE + depth);
        return score;
    }

    Trasposition.WriteHash(board.hashKey, score, depth, bestMove,hashFlag,ply);

    return alpha;
}


#define valWINDOW 50

Move ChessEngine::BestMove(int maxDdepth, const Board& board) {
    auto moves = board.GenerateLegalMoves(board.currentPlayer);
    if (moves.count == 0) {
        // Handle the case where no legal moves are available
        Move empty;
        return empty; // Return an empty move
    }
    clearTables();


    int currentScore = 0;
    int alpha = MIN_SCORE;
    int beta = MAX_SCORE;

    stop = false;

    Move BestLine[max_ply];
    int length;
    for (int i = 1; currentScore < MATE_SCORE && currentScore > -MATE_SCORE; i++) {
        ply = 0;

        currentScore = NegMax(i, board, alpha, beta);


        if (stop) {
            break;
        }


        memcpy(BestLine, pv_table[0], sizeof(BestLine));
        length = pv_length[0];


        int timediff = GetTimeMs() - startTime;

        if (currentScore > MATE_SCORE) {
            printf("info score mate %d depth %d nodes %d time %d nps %d pv ", -(currentScore+MATE_VALUE)/2 - 1, i, count, timediff,count/ (timediff == 0 ? 1 : timediff) *1000);

        }
        else if (currentScore < -MATE_SCORE)
        {
            printf("info score mate %d depth %d nodes %d time %d nps %d pv ", (MATE_VALUE - currentScore)/2 + 1, i, count, GetTimeMs() - startTime, count / (timediff == 0 ? 1 : timediff) * 1000);

        }
        else {
            printf("info score cp %d depth %d nodes %d time %d nps %d pv ", currentScore, i, count, GetTimeMs() - startTime, count / (timediff == 0 ? 1 : timediff) * 1000);
        }

        for (int j = 0; j < length; j++) {
            printf("%s ", BestLine[j].to_str().c_str());
        }
        printf("\n");
        if (currentScore <= alpha || currentScore >= beta) {
            alpha = MIN_SCORE;
            beta = MAX_SCORE;
            continue;
        }




        alpha = currentScore - valWINDOW;
        beta = currentScore + valWINDOW;
    }

    printf("\nbestmove %s\n", BestLine[0].to_str().c_str());
    return BestLine[0];
}


int ChessEngine::Perft(int depth, const Board& board) {
    if (depth == 0) {
        return 1; // Leaf node, return 1
    }

    int nodes = 0;

    auto moves = board.GenerateLegalMoves(board.currentPlayer);

    for (int i = 0; i < moves.count; i++) {
        Board newboard = board;
        newboard.movePiece(moves.moves[i]);

        if ((newboard.isKingAttacked(board.currentPlayer))) {
            // Move doesn't result in own king being in check
            continue;

        }

        if (moves.moves[i].getFlags() == QUEEN_CASTLE || moves.moves[i].getFlags() == KING_CASTLE) {
            if (board.isKingAttacked(board.currentPlayer)) {
                continue;
            }

        }

        // Recursively count the number of nodes at the next depth
        nodes += Perft(depth - 1, newboard);
    }

    return nodes;
}

void ChessEngine::RunPerftTest(int depth, const Board& board) {
    auto start = std::chrono::high_resolution_clock::now();

    int nodes = Perft(depth, board);

    double totaltime = (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start)).count();
    std::cout << "Perft Test Results:\n";
    std::cout << "time: " << totaltime << std::endl;
    std::cout << "Nodes: " << nodes << "\n";
    std::cout << "NPS: " << nodes / totaltime << "\n";
}



void ChessEngine::clearTables() {
    memset(history_moves, 0, sizeof(history_moves));
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(pv_length, 0, sizeof(pv_length));
    memset(pv_table, 0, sizeof(pv_table));
    count = 0;
    ply = 0;
}