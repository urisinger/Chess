#include "engine/ChessEngine.h"





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


bool ChessEngine::IsRepetition(uint64_t hash) {
    for (int i = 0; i < repetitionIndex; i++) {
        if (repetitionTable[i] == hash) {
            return true;
        }
    }
    return false;
}

int ChessEngine::score_move(Move move) {

    if (move == pv_table[0][ply]) {
        return 100000;
    }
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




inline bool ChessEngine::compareMoves(Move moveA, Move moveB) {
    return score_move(moveA) > score_move(moveB);
}


int ChessEngine::quiescence(const Board& board, int alpha, int beta) {
    count++;

    int standPat = board.eval; // Evaluate the current position without considering captures or promotions
    int movesSearched = 0;

    if (standPat >= beta)
        return beta;




    alpha = std::max(alpha, standPat); // Update alpha with the maximum value between alpha and standPat for the maximizing player

    LegalMoves captures = board.GenerateCaptureMoves(board.currentPlayer); // Generate all capture moves

    for (int i = 0; i < captures.count; i++) {

        int minIndex = i;
        for (int j = i + 1; j < captures.count; j++) {
            if (compareMoves(captures.moves[j], captures.moves[minIndex])) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            std::swap(captures.moves[i], captures.moves[minIndex]);
        }


        Board newboard = board;
        newboard.MakeMove(captures.moves[i]);

        if ((newboard.isKingAttacked(board.currentPlayer))) {
            // Move doesn't result in own king being in check
            continue;
        }    


        if (captures.moves[i].getFlags() == QUEEN_CASTLE || captures.moves[i].getFlags() == KING_CASTLE) {

            // Get the starting and ending positions for the castle move
            const int startPos = captures.moves[i].getFrom();
            const int endPos = captures.moves[i].getTo();

            // Check the squares between the starting and ending positions
            const int minPos = std::min(startPos, endPos);
            const int maxPos = std::max(startPos, endPos);

            // Iterate over the squares between the starting and ending positions
            for (int square = minPos; square < maxPos; square++) {
                if (board.isSqaureAttacked(board.currentPlayer, square)) {
                    // Square is attacked, so it is not safe to castle
                    goto skipPlay;
                }
            }
        }

        if (stop) {
            return STOPPED;
        }

        ply++;
        int score = -quiescence(newboard, -beta, -alpha); // Recursively evaluate the capture move with the opposite maximizingPlayer flag
        ply--;



        movesSearched++;

        if (score > alpha) {
            alpha = score;
        }

        if (score >= beta) {
            return beta; // Perform a cutoff if alpha is greater than or equal to beta
        }

    skipPlay:;
    }



    return alpha;
}


const int fmargin[4] = { 0, 200, 300, 700 };
int ChessEngine::NegMax(int depth, const Board& board, int alpha, int beta) {
    pv_length[ply] = ply;

    if (count % 2048) {
        communicate();
    }

    if (ply && IsRepetition(board.hashKey))
    {
        return 0;
    }

    int score;

    bool in_check = board.isKingAttacked(board.currentPlayer);

    depth += in_check;

    if (depth <= 0) {
        // Evaluate the current board position and return the evaluation score
        score = quiescence(board, alpha, beta);
        return score;
    }




    HashFlags hashFlag = HASH_ALPHA;
    int movesSearched = 0;

    THash entry;
    Move bestMove;

    bool isPv = (beta - alpha > 1);


    bool wasHit = Table.ProbeHash(board.hashKey, &entry, ply);
    if (wasHit) {
        bestMove = entry.bestMove;
        if (!isPv && entry.depth >= depth)
        {
            score = entry.score;
            if (entry.flag == HASH_EXSACT) {
                return score;
            }
            if (entry.flag == HASH_ALPHA && score <= alpha) {
                return alpha;
            }
            if (entry.flag == HASH_BETA && score >= beta) {
                return beta;
            }
        }
    }





    bool IsPrunable = depth <= 3 && !in_check && !isPv;


    if (!in_check && ply && !isPv) {

        if (!wasHit) {
            score = board.eval;
        }

        if (depth <= 5 && score >= beta && score - (depth * depth * 20) >= beta)
        {
            return score;
        }

        Board nullBoard = board;

        nullBoard.MakeNullMove();

        char R = 2;

        ply += R + 1;
        int score = -NegMax(depth - 1 - R, nullBoard, -beta, -beta + 1);
        ply -= R + 1;

        if (score >= beta)
            return beta;

    }



    LegalMoves moves = board.GenerateLegalMoves(board.currentPlayer);



    for (int i = 0; i < moves.count; i++) {

        int minIndex = i;
        for (int j = i + 1; j < moves.count; j++) {
            if (moves.moves[j] == bestMove || (compareMoves(moves.moves[j], moves.moves[minIndex]) && moves.moves[minIndex] != bestMove)) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            std::swap(moves.moves[i], moves.moves[minIndex]);
        }


        Board newboard = board;
        newboard.MakeMove(moves.moves[i]);

        if ((newboard.isKingAttacked(board.currentPlayer))) {
            // Move doesn't result in own king being in check
            goto skipPlay;
        }

        if (moves.moves[i].getFlags() == QUEEN_CASTLE || moves.moves[i].getFlags() == KING_CASTLE) {

            if (board.isKingAttacked(board.currentPlayer)) {
                goto skipPlay;
            }

            // Get the starting and ending positions for the castle move
            const int startPos = moves.moves[i].getFrom();
            const int endPos = moves.moves[i].getTo();

            // Check the squares between the starting and ending positions
            const int minPos = std::min(startPos, endPos);
            const int maxPos = std::max(startPos, endPos);

            // Iterate over the squares between the starting and ending positions
            for (int square = minPos; square < maxPos; square++) {
                if (newboard.isSqaureAttacked(board.currentPlayer, square)) {
                    // Square is attacked, so it is not safe to castle
                    goto skipPlay;
                }
            }
        }

        if (IsPrunable && movesSearched && moves.moves[i].getFlags() < 4 && !newboard.isKingAttacked(newboard.currentPlayer) && newboard.eval + fmargin[depth] <= alpha) {
            goto skipPlay;
        }

        if (stop) {
            return STOPPED;
        }

        if (movesSearched == 0) {
            ply++;
            repetitionIndex++;
            repetitionTable[repetitionIndex] = newboard.hashKey;
            score = -NegMax(depth - 1, newboard, -beta, -alpha);
            ply--;
            repetitionIndex--;
        }
        else {
            if (movesSearched >= fullDepthMoves && depth >= reductionLimits && !in_check && moves.moves[i].getFlags() < 4) {
                ply += 2;
                repetitionIndex += 2;
                repetitionTable[repetitionIndex] = newboard.hashKey;
                score = -NegMax(depth - 2, newboard, -alpha - 1, -alpha);
                ply -= 2;
                repetitionIndex -= 2;


            }
            else
                score = alpha + 1;

            if (score > alpha) {
                ply++;
                repetitionIndex++;
                repetitionTable[repetitionIndex] = newboard.hashKey;
                score = -NegMax(depth - 1, newboard, -beta, -alpha);
                ply--;
                repetitionIndex--;
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
            Table.WriteHash(board.hashKey, score, depth, bestMove, HASH_BETA, ply);
            killer_moves[1][ply] = killer_moves[0][ply];
            killer_moves[0][ply] = moves.moves[i];
            return beta;
        }

    skipPlay:;
    }

    if (movesSearched == 0) {
        // Handle the case where no legal moves are available
        score = in_check * -(MATE_VALUE + depth);
    }

    Table.WriteHash(board.hashKey, score, depth, bestMove, hashFlag, ply);

    return alpha;
}

Move ChessEngine::BestMove(int maxDepth, const Board& board) {
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

    int length = 0;
    for (int i = 1; currentScore < MATE_SCORE && currentScore > -MATE_SCORE && i <= maxDepth; i++) {
        ply = 0;

        currentScore = NegMax(i, board, alpha, beta);


        if (stop) {
            break;
        }



        memcpy(BestLine, pv_table[0], sizeof(Move)*pv_length[0]);
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
        newboard.MakeMove(moves.moves[i]);

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
    auto start = GetTimeMs();

    int nodes = Perft(depth, board);

    double totaltime = GetTimeMs()-start;
    std::cout << "Perft Test Results:\n";
    std::cout << "time: " << totaltime << std::endl;
    std::cout << "Nodes: " << (nodes ) << "\n";
    std::cout << "NPS: " << (nodes)/ totaltime << "\n";
}



void ChessEngine::clearTables() {
    memset(history_moves, 0, sizeof(history_moves));
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(pv_length, 0, sizeof(pv_length));
    memset(pv_table, 0, sizeof(pv_table));
    count = 0;
    ply = 0;
}