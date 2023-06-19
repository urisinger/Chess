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

Move ChessEngine::killer_moves[2][max_ply];

int ChessEngine::history_moves[12][64] = { 0 };

int ChessEngine::pv_length[max_ply] = { 0 };
Move ChessEngine::pv_table[max_ply][max_ply];

int ChessEngine::count;


int ChessEngine::ply;

bool ChessEngine::quit = false;

HashTable ChessEngine::Trasposition(HASH_SIZE);

bool ChessEngine::stop;

int ChessEngine::startTime;

double ChessEngine::maxTime;

int ChessEngine::offset;


int GetTimeMs() {
#ifdef WIN32
    return GetTickCount();
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
#endif
}

bool InputWaiting() {
#ifdef _WIN32
    return _kbhit();
#else
    // Code for non-Windows platforms
    return std::cin.rdbuf()->in_avail() > 0;
#endif
}

void ReadInput() {
    if (InputWaiting()) {
        // Set the input stream to be unbuffered to improve performance
        std::cin.tie(nullptr);
        std::cin.sync_with_stdio(false);

        // Read input using std::getline directly from std::cin
        std::string input;
        std::getline(std::cin, input);

        if (!input.empty()) {
            ChessEngine::stop = true;
            if (input == "quit") {
                ChessEngine::quit = true;
            }
        }

        // Restore the input stream buffering settings
        std::cin.tie(nullptr);
        std::cin.sync_with_stdio(true);
    }
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




int score_move(Move move) {


    if (move.getCapturedPiece() != EMPTY) {
        // Compare capture moves
        //translator is just a shortcut i use cuz  i use the vaalue of the piece enum for rendering too, just treat it as the pieces ordered by value
        return (6 - translator[move.getPiece() - 1]) + (translator[move.getCapturedPiece() - 1]) * 100 + 10000;
    }
    else {


        if (ChessEngine::killer_moves[0][ChessEngine::ply] == move) {
            return 9000;
        }

        if (ChessEngine::killer_moves[1][ChessEngine::ply] == move) {
            return 8000;
        }

        return ChessEngine::history_moves[(move.getPiece() - 1) * (move.getColor() + 1)][move.getTo()];

    }


}




int compareMoves(Move moveA, Move moveB, Move bestMove) {
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
void swapMoves(Move& moveA, Move& moveB) {
    Move temp = moveA;
    moveA = moveB;
    moveB = temp;
}

int partitionMoves(Move* moves, int low, int high, Move bestMove) {
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
void quicksortMoves(Move* moves, int low, int high, Move bestMove) {
    if (low < high) {
        int pivotIndex = partitionMoves(moves, low, high, bestMove);
        quicksortMoves(moves, low, pivotIndex - 1, bestMove);
        quicksortMoves(moves, pivotIndex + 1, high, bestMove);
    }
}



void sortMoves(LegalMoves& legalMoves, Move bestMove) {
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
    if (stop) {
        return STOPPED;
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
            if (stop) {
                return STOPPED;
            }
            return beta; // Perform a cutoff if alpha is greater than or equal to beta
        }
    }

    if (stop) {
        return STOPPED;
    }


    return alpha;
}


const int ChessEngine::reductionLimits = 3;
const int ChessEngine::fullDepthMoves = 4;

#define R 2

int ChessEngine::Minimax(int depth, const Board& board, int alpha, int beta) {
    pv_length[ply] = ply;

    if (count % 2048) {
        communicate();
    }

    if (stop) {
        return STOPPED;
    }


    int score;

    HashFlags hashFlag = HASH_ALPHA;
    int movesSearched = 0;

    Move bestMove;
    
    int pv_node = (beta - alpha > 1);

    score = Trasposition.ReadHash(board.hashKey, alpha, beta, &bestMove , depth + offset);
    if (score != NO_HASH_ENTRY && !pv_node)
    {
        return score;
    }

    bool in_check = board.isKingAttacked(board.currentPlayer);

    depth += in_check;
    if (depth == 0) {
        // Evaluate the current board position and return the evaluation score
        score = quiescence(board, alpha, beta);
        if (stop) {
            return STOPPED;
        }
        return score;
    }

    if (depth >= R + 1 && !in_check && ply) {
        Board nullBoard = board;

        nullBoard.currentPlayer = nullBoard.currentPlayer == WHITE ? BLACK : WHITE;

        if (nullBoard.enPassantSquare != -1) {
            nullBoard.hashKey ^= Bitboard::enPeasentKeys[nullBoard.enPassantSquare];
        }
        nullBoard.enPassantSquare = -1;

        nullBoard.hashKey ^= Bitboard::SideKey;

        ply += R + 1;
        int score = -Minimax(depth - 1 - R, nullBoard, -beta, -beta + 1);
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

        }
        

        if (stop) {
            return STOPPED;
        }

        if (movesSearched == 0) {
            ply++;
            score = -Minimax(depth - 1, newboard, -beta, -alpha);
            ply--;
           // *bestMoveP = bestMove;
        }
        else {
            if (movesSearched >= fullDepthMoves && depth >= reductionLimits && !in_check && moves.moves[i].getFlags() < 4) {
                ply += 2;
                score = -Minimax(depth - 2, newboard, -alpha - 1, -alpha);
                ply -= 2;

            }
            else
                score = alpha + 1;

            if (score > alpha) {
                ply++;
                score = -Minimax(depth - 1, newboard, -alpha - 1, -alpha);
                ply--;


                if (score > alpha && score < beta) {
                    ply++;
                    score = -Minimax(depth - 1, newboard, -beta, -alpha);
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
            Trasposition.WriteHash(board.hashKey, score, depth + offset,bestMove, HASH_BETA);
            killer_moves[1][ply] = killer_moves[0][ply];
            killer_moves[0][ply] = moves.moves[i];
            return beta;
        }

    }

    if (stop) {
        return STOPPED;
    }

    if (movesSearched == 0) {
        // Handle the case where no legal moves are available
        score = in_check * (!board.currentPlayer * 2 - 1) * (-MATE_VALUE + ply);
        Trasposition.WriteHash(board.hashKey, score, depth + offset, bestMove, hashFlag);
        return score;
    }

    Trasposition.WriteHash(board.hashKey, score, depth + offset, bestMove,hashFlag);

    return alpha;
}


#define valWINDOW 50

Move ChessEngine::BestMove(int maxDdepth) {
    auto moves = curBoard->GenerateLegalMoves(curBoard->currentPlayer);
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
    for (int i = 1; currentScore < MATE_SCORE && currentScore > -MATE_SCORE && i < maxDdepth; i++) {

        currentScore = Minimax(i, *curBoard, alpha, beta);


        if (stop) {
            break;
        }

        memcpy(BestLine, pv_table[0], sizeof(BestLine));
        length = pv_length[0];


        printf("info score cp %d depth %d nodes %d time %d ", currentScore, i, count, GetTimeMs()- startTime);

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

void ChessEngine::RunPerftTest(int depth) {
    auto start = std::chrono::high_resolution_clock::now();

    int nodes = Perft(depth, *curBoard);

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