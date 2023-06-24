#include "BitBoard.h"
#include <algorithm>

// pawn positional score
const int pawn_score[64] =
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knight_score[64] =
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const int bishop_score[64] =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0

};

// rook positional score
const int rook_score[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
const int king_score[64] =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   0,  -5,  -5,   5,   5,   0,
     0,   0,  5,   0, -15,   5,  10,   0
};


Board::Board(const std::string& fen) :
    whitePawns(0),
    whiteKnights(0),
    whiteBishops(0),
    whiteRooks(0),
    whiteQueens(0),
    whiteKing(0),
    blackPawns(0),
    blackKnights(0),
    blackBishops(0),
    blackRooks(0),
    blackQueens(0),
    blackKing(0),
    whitePieces(0),
    blackPieces(0),
    halfMoveClock(0),
    fullMoveNumber(0),
    enPassantSquare(-1),
    currentPlayer(WHITE),
    castleFlags(0)
{
    ParseFen(fen);
}


void Board::ParseFen(const std::string& fen) {
    whitePawns = 0,
        whiteKnights = 0,
        whiteBishops = 0,
        whiteRooks = 0,
        whiteQueens = 0,
        whiteKing = 0,
        blackPawns = 0,
        blackKnights = 0,
        blackBishops = 0,
        blackRooks = 0,
        blackQueens = 0,
        blackKing = 0,
        whitePieces = 0,
        blackPieces = 0,
        halfMoveClock = 0,
        fullMoveNumber = 0,
        enPassantSquare = -1,
        currentPlayer = WHITE,
        castleFlags = 0;

    std::istringstream iss(fen);
    std::string token;

    // Get the piece positions part of the FEN
    if (!std::getline(iss, token, ' '))
        return;  // FEN is invalid or incomplete

    int rank = 7;
    int file = 0;

    for (char c : token) {
        if (c == '/') {
            // Skip to the next rank
            rank--;
            file = 0;
        }
        else if (std::isdigit(c)) {
            // Skip empty squares
            file += c - '0';
        }
        else {
            // Set the piece in the board
            Color color = std::islower(c) ? BLACK : WHITE;
            Piece piece = pieceFromChar(std::tolower(c));
            setSquare(rank * 8 + file, color, piece);
            file++;
        }

        if (rank < 0)
            break;  // Board is fully initialized
    }



    // Parse the side to move field
    if (!std::getline(iss, token, ' '))
        return;  // FEN is invalid or incomplete

    currentPlayer = (token == "w") ? WHITE : BLACK;

    // Parse the castling rights field
    if (!std::getline(iss, token, ' '))
        return;  // FEN is invalid or incomplete

    castleFlags = 0;
    for (char c : token) {
        switch (c) {
        case 'K': castleFlags |= WHITE_KINGSIDE_CASTLING; break;
        case 'Q': castleFlags |= WHITE_QUEENSIDE_CASTLING; break;
        case 'k': castleFlags |= BLACK_KINGSIDE_CASTLING; break;
        case 'q': castleFlags |= BLACK_QUEENSIDE_CASTLING; break;
        default: break;
        }
    }

    // Parse the en passant target square field
    if (!std::getline(iss, token, ' '))
        return;  // FEN is invalid or incomplete

    enPassantSquare = (token == "-") ? ' ' : token[0];

    // Parse the halfmove clock field
    if (!std::getline(iss, token, ' '))
        return;  // FEN is invalid or incomplete

    halfMoveClock = std::stoi(token);

    // Parse the fullmove number field
    if (!std::getline(iss, token))
        return;  // FEN is invalid or incomplete

    fullMoveNumber = std::stoi(token);

    hashKey = generateHashKey();

    eval = SlowEval();
}

void Board::getBoard(int board[64]) {
    std::fill(board, board + 64, 0);


    // Set the board array based on the piece positions in the Board
    for (int square = 0; square < 64; ++square) {
        if (isSquareOccupied(square)) {
            Piece piece = getPiece(square);
            Color color = (getWhitePiece(piece) & (1ULL << square)) != 0 ? WHITE : BLACK;
            int value = (color == WHITE) ? static_cast<int>(piece) : -static_cast<int>(piece);
            board[square] = value;
        }
    }
}


void Board::clearSquare(std::uint32_t square) {
    const std::uint64_t mask = ~(1ULL << square);
    whitePawns &= mask;
    whiteKnights &= mask;
    whiteBishops &= mask;
    whiteRooks &= mask;
    whiteQueens &= mask;
    whiteKing &= mask;
    blackPawns &= mask;
    blackKnights &= mask;
    blackBishops &= mask;
    blackRooks &= mask;
    blackQueens &= mask;
    blackKing &= mask;

    whitePieces &= mask;
    blackPieces &= mask;
}

bool Board::isSquareOccupied(std::uint32_t square) const {
    const std::uint64_t mask = 1ULL << square;
    return (blackPieces & mask) || (whitePieces & mask);
}

Piece Board::getPiece(std::uint32_t square) const {
    const std::uint64_t mask = 1ULL << square;

    if ((whitePawns | blackPawns) & mask) {
        return PAWN;
    }
    else if ((whiteKnights | blackKnights) & mask) {
        return KNIGHT;
    }
    else if ((whiteBishops | blackBishops) & mask) {
        return BISHOP;
    }
    else if ((whiteRooks | blackRooks) & mask) {
        return ROOK;
    }
    else if ((whiteQueens | blackQueens) & mask) {
        return QUEEN;
    }
    else if ((whiteKing | blackKing) & mask) {
        return KING;
    }
    else {
        return EMPTY;
    }
}


std::uint64_t& Board::getWhitePiece(Piece piece) {
    switch (piece) {
    case PAWN: return whitePawns;
    case KNIGHT: return whiteKnights;
    case BISHOP: return whiteBishops;
    case ROOK: return whiteRooks;
    case QUEEN: return whiteQueens;
    case KING: return whiteKing;
    default: throw std::invalid_argument("Invalid piece");
    }
}

std::uint64_t& Board::getBlackPiece(Piece piece) {
    switch (piece) {
    case PAWN: return blackPawns;
    case KNIGHT: return blackKnights;
    case BISHOP: return blackBishops;
    case ROOK: return blackRooks;
    case QUEEN: return blackQueens;
    case KING: return blackKing;
    default: throw std::invalid_argument("Invalid piece");
    }
}

void inline Board::setSquare(std::uint32_t square, Color color, Piece piece) {
    ((color == WHITE) ? getWhitePiece(piece) : getBlackPiece(piece)) |= 1ULL << square;
    ((color == WHITE) ? whitePieces : blackPieces) |= 1ULL << square;
}

bool Board::isOccupiedByColor(std::int32_t square, Color color) const {
    const std::uint64_t colorPieces = color == WHITE ? whitePieces : blackPieces;
    return (colorPieces & (1ULL << square)) != 0;
}

Piece Board::pieceFromChar(char c) const {
    switch (c) {
    case 'p': return PAWN;
    case 'n': return KNIGHT;
    case 'b': return BISHOP;
    case 'r': return ROOK;
    case 'q': return QUEEN;
    case 'k': return KING;
    default: return PAWN;  // Default to pawn (invalid case)
    }
}

std::uint64_t Board::getColorPieces(Color color) const {
    const std::uint64_t colorPieces = color == WHITE ? whitePieces : blackPieces;

    return colorPieces;
}

void Board::MakeNullMove() {
    currentPlayer = currentPlayer == WHITE ? BLACK : WHITE;
    eval *= -1;

    if (enPassantSquare != -1) {
        hashKey ^= Masks::enPeasentKeys[enPassantSquare];
    }
    enPassantSquare = -1;

    hashKey ^= Masks::SideKey;
}

void Board::MakeMove(const Move& move) {
    const unsigned int to = move.getTo();
    const unsigned int from = move.getFrom();
    const Color color = move.getColor();
    const Piece piece = move.getPiece();
    const Piece capture = move.getCapturedPiece();
    const unsigned int flags = move.getFlags();

    clearSquare(to);
    setSquare(to, color, piece);
    clearSquare(from);


    if (capture != EMPTY) {
        hashKey ^= Masks::pieceKeys[6 * color + capture - 1][to];
    }

    hashKey ^= Masks::pieceKeys[6 * !color + piece - 1][to];

    if (flags == PROMOTE) {
        hashKey ^= Masks::pieceKeys[6 * !color + PAWN - 1][from];

        eval -= pawn_score[from ^ (56 * color)];
        eval -= 100;

        switch (piece) {
        case KNIGHT:
            eval += 300;
            eval += knight_score[to ^ (56 * color)];
            break;
        case BISHOP:
            eval += 325;
            eval += bishop_score[to ^ (56 * color)];
            break;
        case ROOK:
            eval += 500;
            eval += rook_score[to ^ (56 * color)];
            break;
        case QUEEN:
            eval += 900;
            break;
        }
    }
    else {
        hashKey ^= Masks::pieceKeys[6 * !color + piece - 1][from];

        switch (piece) {
        case PAWN:
            eval -= pawn_score[from ^ (56 * color)];
            eval += pawn_score[to ^ (56 * color)];
            break;
        case KNIGHT:
            eval -= knight_score[from ^ (56 * color)];
            eval += knight_score[to ^ (56 * color)];
            break;
        case BISHOP:
            eval -= bishop_score[from ^ (56 * color)];
            eval += bishop_score[to ^ (56 * color)];
            break;
        case ROOK:
            eval -= rook_score[from ^ (56 * color)];
            eval += rook_score[to ^ (56 * color)];
            break;
        case KING:
            eval -= king_score[from ^ (56 * color)];
            eval += king_score[to ^ (56 * color)];
            break;
        }   
    }



    switch (capture) {
    case PAWN:
        eval += pawn_score[(to ^ (56 * !color))];
        eval += 100;
        break;
    case KNIGHT:
        eval += knight_score[to ^ (56 * !color)];
        eval += 300;
        break;
    case BISHOP:
        eval += bishop_score[to ^ (56 * !color)];
        eval += 325;
        break;
    case ROOK:
        eval += rook_score[to ^ (56 * !color)];
        eval += 500;
        break;
    case QUEEN:
        eval += 900;
        break;
    }








    hashKey ^= Masks::CastleKeys[castleFlags];

    if (flags == KING_CASTLE) {
        if (color == WHITE) {
            // Move the rook from H1 to F1
            const unsigned int rookFrom = 7;
            const unsigned int rookTo = 5;
            castleFlags &= ~(WHITE_KINGSIDE_CASTLING | WHITE_QUEENSIDE_CASTLING);
            setSquare(rookTo, color, ROOK);
            clearSquare(rookFrom);

            eval -= rook_score[rookFrom ^ (56 * color)];
            eval += rook_score[rookTo ^ (56 * color)];

            hashKey ^= Masks::pieceKeys[ROOK - 1][rookFrom];

            hashKey ^= Masks::pieceKeys[ROOK - 1][rookTo];

        }
        else if (color == BLACK) {
            // Move the rook from H8 to F8
            const unsigned int rookFrom = 63;
            const unsigned int rookTo = 61;
            castleFlags &= ~(BLACK_KINGSIDE_CASTLING | BLACK_QUEENSIDE_CASTLING);
            setSquare(rookTo, color, ROOK);
            clearSquare(rookFrom);

            eval -= rook_score[rookFrom ^ (56 * color)];
            eval += rook_score[rookTo ^ (56 * color)];

            hashKey ^= Masks::pieceKeys[6 + ROOK - 1][rookFrom];

            hashKey ^= Masks::pieceKeys[6 + ROOK - 1][rookTo];
        }
    }
    else if (flags == QUEEN_CASTLE) {
        if (color == WHITE) {
            // Move the rook from A1 to D1
            const unsigned int rookFrom = 0;
            const unsigned int rookTo = 3;
            castleFlags &= ~(WHITE_KINGSIDE_CASTLING | WHITE_QUEENSIDE_CASTLING);
            setSquare(rookTo, color, ROOK);
            clearSquare(rookFrom);

            eval -= rook_score[rookFrom ^ (56 * color)];
            eval += rook_score[rookTo ^ (56 * color)];

            hashKey ^= Masks::pieceKeys[ROOK - 1][rookFrom];

            hashKey ^= Masks::pieceKeys[ROOK - 1][rookTo];
        }
        else if (color == BLACK) {
            // Move the rook from A8 to D8
            const unsigned int rookFrom = 56;
            const unsigned int rookTo = 59;
            castleFlags &= ~(BLACK_KINGSIDE_CASTLING | BLACK_QUEENSIDE_CASTLING);

            setSquare(rookTo, color, ROOK);
            clearSquare(rookFrom);

            eval -= rook_score[rookFrom ^ (56 * color)];
            eval += rook_score[rookTo ^ (56 * color)];

            hashKey ^= Masks::pieceKeys[6 + ROOK - 1][rookFrom];

            hashKey ^= Masks::pieceKeys[6 + ROOK - 1][rookTo];
        }
    }

    // Check for king's movement
    if (piece == KING) {
        if (color == WHITE) {
            castleFlags &= ~WHITE_KINGSIDE_CASTLING;
            castleFlags &= ~WHITE_QUEENSIDE_CASTLING;
        }
        else {
            castleFlags &= ~BLACK_KINGSIDE_CASTLING;
            castleFlags &= ~BLACK_QUEENSIDE_CASTLING;
        }
    }

    // Check for rook's movement or capture
    if (piece == ROOK) {
        if (color == WHITE) {
            if (from == 0)
                castleFlags &= ~WHITE_QUEENSIDE_CASTLING;
            else if (from == 7)
                castleFlags &= ~WHITE_KINGSIDE_CASTLING;
        }
        else {
            if (from == 56)
                castleFlags &= ~BLACK_QUEENSIDE_CASTLING;
            else if (from == 63)
                castleFlags &= ~BLACK_KINGSIDE_CASTLING;
        }
    }

    if (capture == ROOK) {
        if (to == 0)
            castleFlags &= ~WHITE_QUEENSIDE_CASTLING;
        else if (to == 7)
            castleFlags &= ~WHITE_KINGSIDE_CASTLING;
        else if (to == 56)
            castleFlags &= ~BLACK_QUEENSIDE_CASTLING;
        else if (to == 63)
            castleFlags &= ~BLACK_KINGSIDE_CASTLING;
    }

    // Handle en passant capture
    if (flags == EN_PASSANT_CAPTURE) {
        const int capturedPawnSquare = to + (color == WHITE ? -8 : 8);

        clearSquare(capturedPawnSquare);

        eval += pawn_score[capturedPawnSquare ^ ( 56 * !color)];
        eval -= pawn_score[to ^ ( 56 * !currentPlayer)];

        hashKey ^= Masks::pieceKeys[6 * currentPlayer + PAWN - 1][capturedPawnSquare];
    }

    // Update the halfMoveClock
    if (piece == PAWN || flags == EMPTY || flags == PROMOTE) {
        halfMoveClock = 0;
    }
    else {
        halfMoveClock++;
    }

    // Update the fullMoveNumber
    if (color == BLACK) {
        fullMoveNumber++;
    }

    if (enPassantSquare != -1) {
        hashKey ^= Masks::enPeasentKeys[enPassantSquare];
    }

    // Update en passant square
    if (piece == PAWN && flags == DOUBLE_PUSH) {
        const std::int32_t pawnDirection = (color == WHITE) ? 1 : -1;
        enPassantSquare = to - (pawnDirection * 8);
        hashKey ^= Masks::enPeasentKeys[enPassantSquare];
    }
    else {
        enPassantSquare = -1; // -1 indicates no en passant square
    }



    hashKey ^= Masks::CastleKeys[castleFlags];

    // Update current player
    currentPlayer = (currentPlayer == WHITE) ? BLACK : WHITE;

    hashKey ^= Masks::SideKey;

    
    eval *= -1;

}

void Board::PrintBoard() const {
    const std::string pieceSymbols = ".PNBRQKpnbrqk";  // Symbolic representation of pieces

    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::uint64_t mask = 1ULL << square;  // Bit shift to create a mask for the current square

            // Check if there is a piece on the current square
            if (whitePawns & mask) {
                std::cout << "P";  // White pawn
            }
            else if (whiteKnights & mask) {
                std::cout << "N";  // White knight
            }
            else if (whiteBishops & mask) {
                std::cout << "B";  // White bishop
            }
            else if (whiteRooks & mask) {
                std::cout << "R";  // White rook
            }
            else if (whiteQueens & mask) {
                std::cout << "Q";  // White queen
            }
            else if (whiteKing & mask) {
                std::cout << "K";  // White king
            }
            else if (blackPawns & mask) {
                std::cout << "p";  // Black pawn
            }
            else if (blackKnights & mask) {
                std::cout << "n";  // Black knight
            }
            else if (blackBishops & mask) {
                std::cout << "b";  // Black bishop
            }
            else if (blackRooks & mask) {
                std::cout << "r";  // Black rook
            }
            else if (blackQueens & mask) {
                std::cout << "q";  // Black queen
            }
            else if (blackKing & mask) {
                std::cout << "k";  // Black king
            }
            else {
                std::cout << ".";  // Empty square
            }
        }
        std::cout << std::endl;
    }
}


LegalMoves Board::GenerateLegalMoves(Color color) const {
    LegalMoves legalMoves;


    generatePawnMoves(color, legalMoves);

    std::uint64_t knights = color == WHITE ? whiteKnights : blackKnights;

    while (knights) {
        const int square = getLSB(knights);

        generateNonSlidingMoves(square, legalMoves, Masks::knightAttack[square], KNIGHT);

        // Clear the least significant bit of the current piece
        knights &= (knights - 1);
    }

    {
        constexpr std::uint64_t WHITE_KINGSIDE_EMPTY = 0x0000000000000060ULL;  // Squares f1 and g1 are empty
        constexpr std::uint64_t BLACK_KINGSIDE_EMPTY = 0x6000000000000000ULL;  // Squares f8 and g8 are empty
        constexpr std::uint64_t WHITE_QUEENSIDE_EMPTY = 0x000000000000000EULL;  // Squares b1, c1, and d1 are empty
        constexpr std::uint64_t BLACK_QUEENSIDE_EMPTY = 0x0E00000000000000ULL;  // Squares b8, c8, and d8 are empty

        std::uint64_t kings = color == WHITE ? whiteKing : blackKing;

        const int square = getLSB(kings);

        generateNonSlidingMoves(square, legalMoves, Masks::kingAttack[square], KING);

        // Check if castling is allowed for the current color
        const bool canCastleKingside = (castleFlags & (color == WHITE ? WHITE_KINGSIDE_CASTLING : BLACK_KINGSIDE_CASTLING)) != 0;
        const bool canCastleQueenside = (castleFlags & (color == WHITE ? WHITE_QUEENSIDE_CASTLING : BLACK_QUEENSIDE_CASTLING)) != 0;

        // Generate kingside castle move
        if (canCastleKingside) {
            // Check if the squares between the king and rook are unoccupied
            const std::uint64_t kingsideEmpty = (color == WHITE) ? WHITE_KINGSIDE_EMPTY : BLACK_KINGSIDE_EMPTY;
            if ((kingsideEmpty & (whitePieces | blackPieces)) == 0) {
                const std::uint32_t target = (color == WHITE) ? 6 : 62;
                legalMoves.emplace_back(square, target, KING_CASTLE, color, KING);
            }
        }

        // Generate queenside castle move
        if (canCastleQueenside) {
            // Check if the squares between the king and rook are unoccupied
            const std::uint64_t queensideEmpty = (color == WHITE) ? WHITE_QUEENSIDE_EMPTY : BLACK_QUEENSIDE_EMPTY;
            if ((queensideEmpty & (whitePieces | blackPieces)) == 0) {
                const std::uint32_t target = (color == WHITE) ? 2 : 58;
                legalMoves.emplace_back(square, target, QUEEN_CASTLE, color, KING);
            }
        }
    }

    std::uint64_t bishops = color == WHITE ? whiteBishops : blackBishops;

    while (bishops) {
        const int square = getLSB(bishops);

        generateSlidingMoves(square, legalMoves, Masks::bishopMasks[square], Masks::bishopMagic[square], Masks::bishopAttacks[square], BISHOP);

        // Clear the least significant bit of the current piece
        bishops &= (bishops - 1);
    }

    std::uint64_t rooks = color == WHITE ? whiteRooks : blackRooks;

    while (rooks) {
        const int square = getLSB(rooks);

        generateSlidingMoves(square, legalMoves, Masks::rookMasks[square], Masks::rookMagic[square], Masks::rookAttacks[square], ROOK);

        // Clear the least significant bit of the current piece
        rooks &= (rooks - 1);
    }

    std::uint64_t queens = color == WHITE ? whiteQueens : blackQueens;

    while (queens) {
        const int square = getLSB(queens);

        generateSlidingMoves(square, legalMoves, Masks::rookMasks[square], Masks::rookMagic[square], Masks::rookAttacks[square], QUEEN);

        generateSlidingMoves(square, legalMoves, Masks::bishopMasks[square], Masks::bishopMagic[square], Masks::bishopAttacks[square], QUEEN);


        // Clear the least significant bit of the current piece
        queens &= (queens - 1);
    }
    return legalMoves;
}

inline void Board::generatePawnMoves(const Color color, LegalMoves& legalMoves) const {
    const std::uint64_t colorPieces = (color == WHITE) ? whitePieces : blackPieces;
    const std::uint64_t oppsitePieces = (color == WHITE) ? blackPieces : whitePieces;

    // Determine the direction based on the pawn color
    const std::int32_t direction = (color == WHITE) ? 1 : -1;

    // Generate pawn moves
    const std::uint64_t pawns = (color == WHITE) ? whitePawns : blackPawns;
    std::uint64_t pawnMoves = ((color == WHITE) ? (pawns << 8) : (pawns >> 8)) & ~(whitePieces | blackPieces);

    while (pawnMoves != 0) {
        const std::int32_t targetSquare = getLSB(pawnMoves);
        const std::int32_t sourceSquare = targetSquare - (direction * 8);

        // Check if the pawn is on the starting rank and can make a double move
        const std::int32_t startingRank = (color == WHITE) ? 1 : 6;
        if ((sourceSquare / 8) == startingRank) {
            const std::int32_t doubleMoveTarget = targetSquare + (direction * 8);
            if (!isSquareOccupied(doubleMoveTarget)) {
                legalMoves.emplace_back(sourceSquare, doubleMoveTarget, DOUBLE_PUSH, color, PAWN);
            }
        }

        // Check if the capture is a pawn promotion
        const std::int32_t promotionRank = (color == WHITE) ? 7 : 0;
        if ((targetSquare / 8) == promotionRank) {
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, KNIGHT);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, BISHOP);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, ROOK);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, QUEEN);
        }
        else {
            // Add pawn move to legalMoves
            legalMoves.emplace_back(sourceSquare, targetSquare, QUIET_MOVE, color, PAWN);
        }

        pawnMoves &= pawnMoves - 1; // Clear the LSB to move to the next move
    }

    // Generate pawn captures
    std::uint64_t pawnLeftCaptures = ((((color == WHITE) ? (pawns << 8) : (pawns >> 8)) >> 1) & ~(0x8080808080808080ULL)) & oppsitePieces;
    std::uint64_t pawnRightCaptures = ((((color == WHITE) ? (pawns << 8) : (pawns >> 8)) << 1) & ~(0x0101010101010101ULL)) & oppsitePieces;

    while (pawnLeftCaptures != 0) {
        const std::int32_t targetSquare = getLSB(pawnLeftCaptures);
        const std::int32_t sourceSquare = targetSquare - (direction * 8) + 1;

        // Check if the capture is a pawn promotion
        const std::int32_t promotionRank = (color == WHITE) ? 7 : 0;
        const Piece capturedPiece = getPiece(targetSquare);
        if ((targetSquare / 8) == promotionRank) {
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, KNIGHT, capturedPiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, BISHOP, capturedPiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, ROOK, capturedPiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, QUEEN, capturedPiece);
        }
        else {
            legalMoves.emplace_back(sourceSquare, targetSquare, CAPTURE, color, PAWN, capturedPiece);
        }

        pawnLeftCaptures &= pawnLeftCaptures - 1; // Clear the LSB to move to the next capture
    }

    while (pawnRightCaptures != 0) {
        const std::int32_t targetSquare = getLSB(pawnRightCaptures);
        const std::int32_t sourceSquare = targetSquare - (direction * 8) - 1;

        // Check if the capture is a pawn promotion
        const std::int32_t promotionRank = (color == WHITE) ? 7 : 0;
        const Piece capturedPiece = getPiece(targetSquare);

        if ((targetSquare / 8) == promotionRank) {
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, KNIGHT, capturedPiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, BISHOP, capturedPiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, ROOK, capturedPiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, QUEEN, capturedPiece);
        }
        else {
            legalMoves.emplace_back(sourceSquare, targetSquare, CAPTURE, color, PAWN, capturedPiece);
        }

        pawnRightCaptures &= pawnRightCaptures - 1; // Clear the LSB to move to the next capture
    }

    // Check en passant
    if (enPassantSquare != -1) {
        const std::int32_t enPassantOffset = (color == WHITE) ? -8 : 8;
        const std::int32_t enPassantFile = (enPassantSquare % 8);

        const std::int32_t leftCaptureSquare = enPassantSquare + enPassantOffset - 1;
        const std::int32_t rightCaptureSquare = enPassantSquare + enPassantOffset + 1;

        if (enPassantFile > 0 && leftCaptureSquare >= 0 && leftCaptureSquare < 64) {
            const std::uint64_t enPassantLeftCaptureMask = 1ULL << leftCaptureSquare;
            if (pawns & enPassantLeftCaptureMask) {
                legalMoves.emplace_back(leftCaptureSquare, enPassantSquare, EN_PASSANT_CAPTURE, color, PAWN, PAWN);
            }
        }

        if (enPassantFile < 7 && rightCaptureSquare >= 0 && rightCaptureSquare < 64) {
            const std::uint64_t enPassantRightCaptureMask = 1ULL << rightCaptureSquare;
            if (pawns & enPassantRightCaptureMask) {
                legalMoves.emplace_back(rightCaptureSquare, enPassantSquare, EN_PASSANT_CAPTURE, color, PAWN, PAWN);
            }
        }
    }
}


inline void Board::generateNonSlidingMoves(const std::int32_t square, LegalMoves& legalMoves, std::uint64_t mask, Piece piece) const {
    const std::uint64_t pieces = (whitePieces & (1ULL << square)) ? whitePieces : blackPieces;
    const std::uint64_t oppsitePieces = (whitePieces & (1ULL << square)) ? blackPieces : whitePieces;

    std::uint64_t captureMoves = (mask & ~pieces) & oppsitePieces;
    std::uint64_t nonCaptureMoves = (mask & ~pieces) & ~oppsitePieces;

    while (captureMoves != 0) {
        const std::uint32_t target = getLSB(captureMoves);
        const Color targetColor = (whitePieces & (1ULL << square)) ? WHITE : BLACK;
        const Piece capturedPiece = getPiece(target);
        legalMoves.emplace_back(square, target, CAPTURE, targetColor, piece, capturedPiece);
        captureMoves &= ~(1ULL << target);
    }

    while (nonCaptureMoves != 0) {
        const std::uint32_t target = getLSB(nonCaptureMoves);
        const Color targetColor = (whitePieces & (1ULL << square)) ? WHITE : BLACK;
        legalMoves.emplace_back(square, target, QUIET_MOVE, targetColor, piece);
        nonCaptureMoves &= ~(1ULL << target);
    }
}


inline void Board::generateSlidingMoves(std::int32_t square, LegalMoves& legalMoves, std::uint64_t mask, std::uint64_t magic_number, std::uint64_t attacks[4096], Piece piece) const {
    std::uint64_t occupancy = ((whitePieces | blackPieces) & mask);

    std::uint64_t index = (occupancy * magic_number) >> (64 - countBits(mask));

    std::uint64_t attack = attacks[index];

    std::uint64_t capture = attack & ((whitePieces & (1ULL << square)) ? blackPieces : whitePieces);

    attack &= ~(whitePieces | blackPieces);
    while (attack != 0) {
        const std::uint32_t target = getLSB(attack);
        const Color targetColor = (whitePieces & (1ULL << square)) ? WHITE : BLACK;
        legalMoves.emplace_back(square, target, QUIET_MOVE, targetColor, piece);
        attack &= ~(1ULL << target);
    }

    while (capture != 0) {
        const std::uint32_t target = getLSB(capture);
        const Color targetColor = (whitePieces & (1ULL << square)) ? WHITE : BLACK;
        const Piece capturedPiece = getPiece(target);
        legalMoves.emplace_back(square, target, CAPTURE, targetColor, piece, capturedPiece);
        capture &= ~(1ULL << target);
    }
}

LegalMoves Board::GenerateCaptureMoves(Color color) const {
    LegalMoves legalMoves;

    generatePawnAttacks(color, legalMoves);


    std::uint64_t knights = color == WHITE ? whiteKnights : blackKnights;

    while (knights) {
        const int square = getLSB(knights);

        generateNonSlidingAttacks(square, legalMoves, Masks::knightAttack[square], KNIGHT);

        // Clear the least significant bit of the current piece
        knights &= (knights - 1);
    }

    std::uint64_t bishops = color == WHITE ? whiteBishops : blackBishops;

    while (bishops) {
        const int square = getLSB(bishops);

        generateSlidingAttacks(square, legalMoves, Masks::bishopMasks[square], Masks::bishopMagic[square], Masks::bishopAttacks[square], BISHOP);

        // Clear the least significant bit of the current piece
        bishops &= (bishops - 1);
    }

    std::uint64_t rooks = color == WHITE ? whiteRooks : blackRooks;

    while (rooks) {
        const int square = getLSB(rooks);

        generateSlidingAttacks(square, legalMoves, Masks::rookMasks[square], Masks::rookMagic[square], Masks::rookAttacks[square], ROOK);

        // Clear the least significant bit of the current piece
        rooks &= (rooks - 1);
    }

    std::uint64_t queens = color == WHITE ? whiteQueens : blackQueens;

    while (queens) {
        const int square = getLSB(queens);

        generateSlidingAttacks(square, legalMoves, Masks::rookMasks[square], Masks::rookMagic[square], Masks::rookAttacks[square], QUEEN);

        generateSlidingAttacks(square, legalMoves, Masks::bishopMasks[square], Masks::bishopMagic[square], Masks::bishopAttacks[square], QUEEN);


        // Clear the least significant bit of the current piece
        queens &= (queens - 1);
    }


    return legalMoves;
}

inline void Board::generateNonSlidingAttacks(const std::int32_t square, LegalMoves& legalMoves, std::uint64_t mask, Piece piece) const {
    const std::uint64_t pieces = (whitePieces & (1ULL << square)) ? whitePieces : blackPieces;
    const std::uint64_t oppsitePieces = (whitePieces & (1ULL << square)) ? blackPieces : whitePieces;

    std::uint64_t captureMoves = (mask & ~pieces) & oppsitePieces;

    while (captureMoves != 0) {
        const std::uint32_t target = getLSB(captureMoves);
        const Color targetColor = (whitePieces & (1ULL << square)) ? WHITE : BLACK;
        const Piece capturedPiece = getPiece(target);
        legalMoves.emplace_back(square, target, CAPTURE, targetColor, piece, capturedPiece);
        captureMoves &= ~(1ULL << target);
    }
}

inline void Board::generatePawnAttacks(const Color color, LegalMoves& legalMoves) const {
    const std::uint64_t oppsitePieces = (color == WHITE) ? blackPieces : whitePieces;

    // Determine the direction based on the pawn color
    const std::int32_t direction = (color == WHITE) ? 1 : -1;

    // Generate pawn moves
    const std::uint64_t pawns = (color == WHITE) ? whitePawns : blackPawns;

    // Generate pawn captures
    std::uint64_t pawnLeftCaptures = ((((color == WHITE) ? (pawns << 8) : (pawns >> 8)) >> 1) & ~(0x8080808080808080ULL)) & oppsitePieces;
    std::uint64_t pawnRightCaptures = ((((color == WHITE) ? (pawns << 8) : (pawns >> 8)) << 1) & ~(0x0101010101010101ULL)) & oppsitePieces;

    while (pawnLeftCaptures != 0) {
        const std::int32_t targetSquare = getLSB(pawnLeftCaptures);
        const std::int32_t sourceSquare = targetSquare - (direction * 8) + 1;

        // Check if the capture is a pawn promotion
        Piece targetpiece = getPiece(targetSquare);
        const std::int32_t promotionRank = (color == WHITE) ? 7 : 0;
        if ((targetSquare / 8) == promotionRank) {
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, KNIGHT, targetpiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, BISHOP, targetpiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, ROOK, targetpiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, QUEEN, targetpiece);
        }
        else {
            legalMoves.emplace_back(sourceSquare, targetSquare, CAPTURE, color, PAWN, targetpiece);
        }

        pawnLeftCaptures &= pawnLeftCaptures - 1; // Clear the LSB to move to the next capture
    }

    while (pawnRightCaptures != 0) {
        const std::int32_t targetSquare = getLSB(pawnRightCaptures);
        const std::int32_t sourceSquare = targetSquare - (direction * 8) - 1;

        Piece targetpiece = getPiece(targetSquare);

        // Check if the capture is a pawn promotion
        const std::int32_t promotionRank = (color == WHITE) ? 7 : 0;

        if ((targetSquare / 8) == promotionRank) {
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, KNIGHT, targetpiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, BISHOP, targetpiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, ROOK, targetpiece);
            legalMoves.emplace_back(sourceSquare, targetSquare, PROMOTE, color, QUEEN, targetpiece);
        }
        else {
            legalMoves.emplace_back(sourceSquare, targetSquare, CAPTURE, color, PAWN, targetpiece);
        }

        pawnRightCaptures &= pawnRightCaptures - 1; // Clear the LSB to move to the next capture
    }

    // Check en passant
    if (enPassantSquare != -1) {
        const std::int32_t enPassantOffset = (color == WHITE) ? -8 : 8;
        const std::int32_t enPassantFile = (enPassantSquare % 8);

        const std::int32_t leftCaptureSquare = enPassantSquare + enPassantOffset - 1;
        const std::int32_t rightCaptureSquare = enPassantSquare + enPassantOffset + 1;

        if (enPassantFile > 0 && leftCaptureSquare >= 0 && leftCaptureSquare < 64) {
            const std::uint64_t enPassantLeftCaptureMask = 1ULL << leftCaptureSquare;
            if (pawns & enPassantLeftCaptureMask) {
                legalMoves.emplace_back(leftCaptureSquare, enPassantSquare, EN_PASSANT_CAPTURE, color, PAWN, PAWN);
            }
        }

        if (enPassantFile < 7 && rightCaptureSquare >= 0 && rightCaptureSquare < 64) {
            const std::uint64_t enPassantRightCaptureMask = 1ULL << rightCaptureSquare;
            if (pawns & enPassantRightCaptureMask) {
                legalMoves.emplace_back(rightCaptureSquare, enPassantSquare, EN_PASSANT_CAPTURE, color, PAWN, PAWN);
            }
        }
    }
}


inline void Board::generateSlidingAttacks(std::int32_t square, LegalMoves& legalMoves, std::uint64_t mask, std::uint64_t magic_number, std::uint64_t attacks[4096], Piece piece) const {
    std::uint64_t occupancy = ((whitePieces | blackPieces) & mask);

    std::uint64_t index = (occupancy * magic_number) >> (64 - countBits(mask));

    std::uint64_t attack = attacks[index];

    std::uint64_t capture = attack & ((whitePieces & (1ULL << square)) ? blackPieces : whitePieces);

    while (capture != 0) {
        const std::uint32_t target = getLSB(capture);
        const Color targetColor = (whitePieces & (1ULL << square)) ? WHITE : BLACK;
        const Piece capturedPiece = getPiece(target);
        legalMoves.emplace_back(square, target, CAPTURE, targetColor, piece, capturedPiece);
        capture &= ~(1ULL << target);
    }
}

std::uint64_t Board::generateNonSlidingMovesAsBits(std::int32_t square, std::uint64_t mask, Piece piece) const {
    const std::uint64_t pieces = (whitePieces & (1ULL << square)) ? whitePieces : blackPieces;
    const std::uint64_t oppositePieces = (whitePieces & (1ULL << square)) ? blackPieces : whitePieces;

    std::uint64_t moves = (mask & ~pieces) & oppositePieces;

    return moves;
}


std::uint64_t Board::generateSlidingMovesAsBits(std::int32_t square, std::uint64_t mask, std::uint64_t magic_number, const std::uint64_t attacks[4096], Piece piece) const {
    std::uint64_t occupancy = ((whitePieces | blackPieces) & mask);
    std::uint64_t index = (occupancy * magic_number) >> (64 - countBits(mask));
    std::uint64_t attack = attacks[index];

    std::uint64_t moves = attack & ~(((whitePieces & (1ULL << square)) ? whitePieces : blackPieces));

    return moves;
}



bool Board::isSqaureAttacked(Color color, int square) const {
    std::uint64_t pawns = color == WHITE ? blackPawns : whitePawns;

    std::uint64_t knights = color == WHITE ? blackKnights : whiteKnights;

    if (color == WHITE) {
        if (square % 8 != 7 && (pawns & (1ULL << (square + 9)))) {
            return true;
        }

        if (square % 8 != 0 && (pawns & (1ULL << (square + 7)))) {
            return true;
        }
    }
    else {
        if (square % 8 != 0 && (pawns & (1ULL << (square - 9)))) {
            return true;
        }

        if (square % 8 != 7 && (pawns & (1ULL << (square - 7)))) {
            return true;
        }
    }

    if (knights & Masks::knightAttack[square]) {
        return true;
    }

    std::uint64_t kings = color == WHITE ? blackKing : whiteKing;

    if (kings & Masks::kingAttack[square]) {
        return true;
    }

    std::uint64_t queens = color == WHITE ? blackQueens : whiteQueens;

    std::uint64_t bishops = color == WHITE ? blackBishops : whiteBishops;

    if ((bishops | queens) & generateSlidingMovesAsBits(square, Masks::bishopMasks[square],
        Masks::bishopMagic[square], Masks::bishopAttacks[square],
        BISHOP)) {
        return true;
    }

    std::uint64_t rooks = color == WHITE ? blackRooks : whiteRooks;

    if ((rooks | queens) & generateSlidingMovesAsBits(square, Masks::rookMasks[square],
        Masks::rookMagic[square], Masks::rookAttacks[square],
        ROOK)) {
        return true;
    }


    return false;
}



bool Board::isKingAttacked(Color color) const {
    int square = getLSB(color == WHITE ? whiteKing : blackKing);


    return isSqaureAttacked(color, square);
}



int Board::SlowEval() const {
    int result = 0;

    std::uint64_t pieces = whiteKnights;
    while (pieces) {
        const int square = getLSB(pieces);
        result += 300;
        result += knight_score[(7 - (square / 8)) * 8 + (square % 8)];
        pieces &= (pieces - 1);
    }

    pieces = whiteBishops;
    while (pieces) {
        const int square = getLSB(pieces);
        result += 325;
        result += bishop_score[(7 - (square / 8)) * 8 + (square % 8)];
        pieces &= (pieces - 1);
    }

    pieces = whiteRooks;
    while (pieces) {
        const int square = getLSB(pieces);
        result += 500;
        result += rook_score[(7 - (square / 8)) * 8 + (square % 8)];
        pieces &= (pieces - 1);
    }

    pieces = whitePawns;
    while (pieces) {
        const int square = getLSB(pieces);
        result += 100;
        result += pawn_score[(7 - (square / 8)) * 8 + (square % 8)];
        pieces &= (pieces - 1);
    }

    pieces = whiteQueens;
    while (pieces) {
        const int square = getLSB(pieces);
        result += 900;
        pieces &= (pieces - 1);
    }

    pieces = blackKnights;
    while (pieces) {
        const int square = getLSB(pieces);
        result -= 300;
        result -= knight_score[square];
        pieces &= (pieces - 1);
    }

    pieces = blackBishops;
    while (pieces) {
        const int square = getLSB(pieces);
        result -= 325;
        result -= bishop_score[square];
        pieces &= (pieces - 1);
    }

    pieces = blackRooks;
    while (pieces) {
        const int square = getLSB(pieces);
        result -= 500;
        result -= rook_score[square];
        pieces &= (pieces - 1);
    }

    pieces = blackPawns;
    while (pieces) {
        const int square = getLSB(pieces);
        result -= 100;
        result -= pawn_score[square];
        pieces &= (pieces - 1);
    }

    pieces = blackQueens;
    while (pieces) {
        const int square = getLSB(pieces);
        result -= 900;
        pieces &= (pieces - 1);
    }

    if (whiteKing) {
        const int square = getLSB(whiteKing);
        result += 10000; // Assuming a fixed value for the white king
        result += king_score[(7 - (square / 8)) * 8 + (square % 8)];
    }

    if (blackKing) {
        const int square = getLSB(blackKing);
        result -= 10000; // Assuming a fixed value for the black king
        result -= king_score[square]; 
    }

    return currentPlayer == WHITE ? result : -result;
}

std::uint64_t Board::generateHashKey() {
    std::uint64_t key = 0ULL;

    std::uint64_t pieces = whitePieces;

    while (pieces) {
        int square = getLSB(pieces);
        Piece piece = getPiece(square);
        key ^= Masks::pieceKeys[piece - 1][square];
        pieces &= pieces - 1;
    }

    pieces = blackPieces;
    while (pieces) {
        int square = getLSB(pieces);
        Piece piece = getPiece(square);
        key ^= Masks::pieceKeys[6 + piece - 1][square];
        pieces &= pieces - 1;
    }

    if (enPassantSquare != -1) {
        key ^= Masks::enPeasentKeys[enPassantSquare];
    }

    key ^= Masks::CastleKeys[castleFlags];

    key ^= !currentPlayer * Masks::SideKey;

    return key;
}