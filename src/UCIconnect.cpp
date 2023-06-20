#include "UCIconnect.h"
#define INPUTBUFFER 400 * 6
#define NAME "FUN_CHESS"






void UCIconnection::Loop() {

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char line[INPUTBUFFER];
    printf("id name %s\n", NAME);
    printf("id author Uri Singer\n");
    printf("option name Hash type spin default 64 min 4 max %d\n", HASH_MAX);
    printf("option name Book type check default true\n");
    printf("uciok\n");

    int MB = 64;

    while (TRUE) {
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);
        if (!fgets(line, INPUTBUFFER, stdin))
            continue;

        if (line[0] == '\n')
            continue;

        if (!strncmp(line, "isready", 7)) {
            printf("readyok\n");
            continue;
        }
        else if (!strncmp(line, "position", 8)) {
            ParsePos(line);
        }
        else if (!strncmp(line, "ucinewgame", 10)) {
            ParsePos("position startpos\n");
        }
        else if (!strncmp(line, "go", 2)) {
            printf("Seen Go..\n");
            ParseGo(line);
            _engine.stop = false;
        }
        else if (!strncmp(line, "quit", 4)) {
            _engine.stop = true;
            break;
        }
        else if (!strncmp(line, "uci", 3)) {
            printf("id name %s\n", NAME);
            printf("id author Uri Singer\n");
            printf("uciok\n");
        }
        else if (!strncmp(line, "setoption name Hash value ", 26)) {
            sscanf(line, "%*s %*s %*s %*s %d", &MB);
            if (MB < 4) MB = 4;
            if (MB > HASH_MAX) MB = HASH_MAX;
            printf("Set Hash to %d MB\n", MB);
            _engine.Trasposition.Resize((1000000 / sizeof(THash)) * MB);
        }
        if (_engine.stop) break;
    }
}


void UCIconnection::ParsePos(char* lineIn) {

    lineIn += 9;
    char* ptrChar = lineIn;

    if (strncmp(lineIn, "startpos", 8) == 0) {
        _board.ParseFen();
    }
    else {
        ptrChar = strstr(lineIn, "fen");
        if (ptrChar == NULL) {
            _board.ParseFen();
        }
        else {
            ptrChar += 4;
            _board.ParseFen(ptrChar);
        }
    }

    ptrChar = strstr(lineIn, "moves");
    Move move;

    if (ptrChar != NULL) {
        ptrChar += 6;
        while (*ptrChar) {
            move = ParseMove(ptrChar);
            _board.movePiece(move);
            while (*ptrChar && *ptrChar != ' ') ptrChar++;
            while ((*ptrChar == ' ' || *ptrChar == '\n')) ptrChar++;
        }
    }
    _board.PrintBoard();
}


void UCIconnection::ParseGo(char* line) {

    int depth = 100000, movestogo = 30, movetime = -1;
    int time = -1, inc = 0;
    char* ptr = NULL;
    _engine.maxTime = 1000000;

    if ((ptr = strstr(line, "infinite"))) {
        ;
    }

    if ((ptr = strstr(line, "binc")) && _board.currentPlayer == BLACK) {
        inc = atoi(ptr + 5);
    }

    if ((ptr = strstr(line, "winc")) && _board.currentPlayer == WHITE) {
        inc = atoi(ptr + 5);
    }

    if ((ptr = strstr(line, "wtime")) && _board.currentPlayer == WHITE) {
        time = atoi(ptr + 6);
    }

    if ((ptr = strstr(line, "btime")) && _board.currentPlayer == BLACK) {
        time = atoi(ptr + 6);
    }

    if ((ptr = strstr(line, "movestogo"))) {
        movestogo = atoi(ptr + 10);
    }

    if ((ptr = strstr(line, "movetime"))) {
        movetime = atoi(ptr + 9);
    }

    if ((ptr = strstr(line, "depth"))) {
        depth = atoi(ptr + 6);
    }

    if (movetime != -1) {
        time = movetime;
        movestogo = 1;
    }

    if (time != -1) {
        time /= movestogo;
        time -= 50;
        _engine.maxTime = time;

    }
    _engine.startTime = GetTimeMs();

    printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
        time, _engine.startTime, _engine.maxTime, depth, (_engine.maxTime != 1000000));
    _engine.BestMove(depth);
}

Move UCIconnection::ParseMove(const std::string& moveString) {

    std::istringstream iss(moveString);
    std::string token;

    // Extract the 'from' position
    if (!std::getline(iss, token, ' ')) {
        throw std::invalid_argument("Invalid move format. Expected 'fromtopromotion'.");
    }

    if (!token.empty() && token.back() == '\n') {
        token.pop_back();
    }

    if (token.length() < 4) {
        throw std::invalid_argument("Invalid move format. Expected 'fromtopromotion'.");
    }

    char fromFile = token[0];
    char fromRank = token[1];
    char toFile = token[2];
    char toRank = token[3];

    if (fromFile < 'a' || fromFile > 'h' || fromRank < '1' || fromRank > '8' ||
        toFile < 'a' || toFile > 'h' || toRank < '1' || toRank > '8') {
        throw std::invalid_argument("Invalid move format. File should be 'a' to 'h' and rank should be '1' to '8'.");
    }

    int from = (fromFile - 'a') + (8 * (fromRank - '1'));
    int to = (toFile - 'a') + (8 * (toRank - '1'));

    // Check if the move is a castling move
    if (from == 4 && to == 6 && _board.getPiece(from) == Piece::KING && _board.currentPlayer == Color::WHITE) {
        return Move(from, to, MoveFlag::KING_CASTLE, _board.currentPlayer, Piece::KING, Piece::EMPTY);
    }
    else if (from == 4 && to == 2 && _board.getPiece(from) == Piece::KING && _board.currentPlayer == Color::WHITE) {
        return Move(from, to, MoveFlag::QUEEN_CASTLE, _board.currentPlayer, Piece::KING, Piece::EMPTY);
    }
    else if (from == 60 && to == 62 && _board.getPiece(from) == Piece::KING && _board.currentPlayer == Color::BLACK) {
        return Move(from, to, MoveFlag::KING_CASTLE, _board.currentPlayer, Piece::KING, Piece::EMPTY);
    }
    else if (from == 60 && to == 58 && _board.getPiece(from) == Piece::KING && _board.currentPlayer == Color::BLACK) {
        return Move(from, to, MoveFlag::QUEEN_CASTLE, _board.currentPlayer, Piece::KING, Piece::EMPTY);
    }

    // Check if the move is an en passant capture
    if (_board.getPiece(from) == Piece::PAWN && _board.getPiece(to) == Piece::EMPTY) {
        if (_board.currentPlayer == Color::WHITE && fromRank == '5' && toRank == '6' && std::abs(fromFile - toFile) == 1) {
            int captureSquare = to - 8;
            if (_board.getPiece(captureSquare) == Piece::PAWN) {
                return Move(from, to, MoveFlag::EN_PASSANT_CAPTURE, _board.currentPlayer, Piece::PAWN, Piece::PAWN);
            }
        }
        else if (_board.currentPlayer == Color::WHITE && fromRank == '4' && toRank == '3' && std::abs(fromFile - toFile) == 1) {
            int captureSquare = to + 8;
            if (_board.getPiece(captureSquare) == Piece::PAWN) {
                return Move(from, to, MoveFlag::EN_PASSANT_CAPTURE, _board.currentPlayer, Piece::PAWN, Piece::PAWN);
            }
        }
    }

    // Check if there is a captured piece on the 'to' square
    Piece capturedPiece = _board.getPiece(to);

    // Extract promotion piece if present
    Piece promotionPiece = Piece::EMPTY;  // Default promotion piece is None
    if (token.length() > 4) {
        char promotionPieceChar = token[4];
        switch (promotionPieceChar) {
        case 'q':
            promotionPiece = Piece::QUEEN;
            break;
        case 'r':
            promotionPiece = Piece::ROOK;
            break;
        case 'b':
            promotionPiece = Piece::BISHOP;
            break;
        case 'n':
            promotionPiece = Piece::KNIGHT;
            break;
        default:
            throw std::invalid_argument("Invalid promotion piece. Valid promotion pieces are 'q', 'r', 'b', 'n'.");
        }
        return Move(from, to, MoveFlag::PROMOTE, _board.currentPlayer, promotionPiece, capturedPiece);
    }

    return Move(from, to, MoveFlag::QUIET_MOVE, _board.currentPlayer, _board.getPiece(from), capturedPiece);
}


