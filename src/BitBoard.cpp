#include "BitBoard.h"



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
    enPassantSquare(0),
    currentPlayer(WHITE),
    castleFlags(0)
{
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
}


void Board::getBoard(int board[64]){
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

Piece Board::getPiece(std::uint32_t square) const{
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


std::uint64_t& Board::getWhitePiece(Piece piece){
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

std::uint64_t& Board::getBlackPiece(Piece piece){
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
    ((color == WHITE) ? getWhitePiece(piece) : getBlackPiece(piece) )|= 1ULL << square;
    ((color == WHITE) ? whitePieces: blackPieces )|= 1ULL << square;
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

void Board::movePiece(const Move& move) {
    const unsigned int to = move.getTo();
    const unsigned int from = move.getFrom();
    const Color color = move.getColor();
    const Piece piece = move.getPiece();
    const unsigned int flags = move.getFlags();

    clearSquare(to);
    setSquare(to, color, piece);
    clearSquare(from);

    // Handle en passant capture
    if (flags == EN_PASSANT_CAPTURE) {
        const int capturedPawnSquare = to + (color == WHITE ? -8 : 8);
        clearSquare(capturedPawnSquare);
    }

    // Update the halfMoveClock
    if (piece == PAWN || isSquareOccupied(to)) {
        halfMoveClock = 0;
    }
    else {
        halfMoveClock++;
    }

    // Update the fullMoveNumber
    if (color == BLACK) {
        fullMoveNumber++;
    }

    // Update en passant square
    if (piece == PAWN && (((to > from) ? (to - from) : (from - to)) == 16)) {
        const std::int32_t pawnDirection = (color == WHITE) ? 1 : -1;
        enPassantSquare = to - (pawnDirection * 8);
    }
    else {
        enPassantSquare = -1; // -1 indicates no en passant square
    }


    // Update current player
    currentPlayer = (currentPlayer == WHITE) ? BLACK : WHITE;
}

std::vector<Move> Board::GenerateLegalMoves(Color color) const {
    std::vector<Move> legalMoves;
    legalMoves.reserve(256);


    generatePawnMoves(color, legalMoves);

    std::uint64_t knights = color == WHITE ? whiteKnights : blackKnights;

    while(knights){
        const int square = getLSB(knights);

        generateNonSlidingMoves(square, legalMoves, Bitboard::knightAttack[square], KNIGHT);

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

        generateNonSlidingMoves(square, legalMoves, Bitboard::kingAttack[square], KING);

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

        generateSlidingMoves(square, legalMoves, Bitboard::bishopMasks[square],Bitboard::bishopMagic[square], Bitboard::bishopAttacks[square], BISHOP);

        // Clear the least significant bit of the current piece
        bishops &= (bishops - 1);
    }

    std::uint64_t rooks = color == WHITE ? whiteRooks : blackRooks;

    while (rooks) {
        const int square = getLSB(rooks);

        generateSlidingMoves(square, legalMoves, Bitboard::rookMasks[square], Bitboard::rookMagic[square], Bitboard::rookAttacks[square], ROOK);

        // Clear the least significant bit of the current piece
        rooks &= (rooks - 1);
    }

    std::uint64_t queens = color == WHITE ? whiteQueens : blackQueens;

    while (queens) {
        const int square = getLSB(queens);

        generateSlidingMoves(square, legalMoves, Bitboard::rookMasks[square], Bitboard::rookMagic[square], Bitboard::rookAttacks[square], QUEEN);

        generateSlidingMoves(square, legalMoves, Bitboard::bishopMasks[square], Bitboard::bishopMagic[square], Bitboard::bishopAttacks[square], QUEEN);


        // Clear the least significant bit of the current piece
        queens &= (queens - 1);
    }

    for (auto it = legalMoves.begin(); it != legalMoves.end(); ) {
        Board movedBoard = *this;
        movedBoard.movePiece(*it);

        const std::uint64_t king = color == WHITE ? movedBoard.whiteKing : movedBoard.blackKing;
        if ((king & movedBoard.GetAttackedPieces(color == WHITE ? BLACK : WHITE)) != 0) {
            it = legalMoves.erase(it);  // Remove the move from the vector
        }
        else {
            ++it;  // Move to the next element
        }
    }

    if (legalMoves.empty()) {
        const std::uint64_t king = color == WHITE ? whiteKing : blackKing;
        if ((king & GetAttackedPieces(color == WHITE ? BLACK : WHITE )) != 0) {
            std::cout << "you LOSE: " << (color == WHITE ? "WHITE" : "BLACK") << std::endl;
        }
        else {
            std::cout << "DRAW!" << std::endl;
        }
    }

    return legalMoves;
}

inline void Board::generatePawnMoves(const Color color, std::vector<Move>& legalMoves) const{
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
            legalMoves.emplace_back(sourceSquare, targetSquare, KNIGHT_PROMOTE_CAPTURE, color, KNIGHT);
            legalMoves.emplace_back(sourceSquare, targetSquare, BISHOP_PROMOTE_CAPTURE, color, BISHOP);
            legalMoves.emplace_back(sourceSquare, targetSquare, ROOK_PROMOTE_CAPTURE, color, ROOK);
            legalMoves.emplace_back(sourceSquare, targetSquare, QUEEN_PROMOTE_CAPTURE, color, QUEEN);
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
        const std::int32_t sourceSquare = targetSquare - (direction * 8)+1;

        // Check if the capture is a pawn promotion
        const std::int32_t promotionRank = (color == WHITE) ? 7 : 0;
        if ((targetSquare / 8) == promotionRank) {
            legalMoves.emplace_back(sourceSquare, targetSquare, KNIGHT_PROMOTE_CAPTURE, color, KNIGHT);
            legalMoves.emplace_back(sourceSquare, targetSquare, BISHOP_PROMOTE_CAPTURE, color, BISHOP);
            legalMoves.emplace_back(sourceSquare, targetSquare, ROOK_PROMOTE_CAPTURE, color, ROOK);
            legalMoves.emplace_back(sourceSquare, targetSquare, QUEEN_PROMOTE_CAPTURE, color, QUEEN);
        }
        else {
            legalMoves.emplace_back(sourceSquare, targetSquare, CAPTURE, color, PAWN);
        }

        pawnLeftCaptures &= pawnLeftCaptures - 1; // Clear the LSB to move to the next capture
    }

    while (pawnRightCaptures != 0) {
        const std::int32_t targetSquare = getLSB(pawnRightCaptures);
        const std::int32_t sourceSquare = targetSquare - (direction * 8)-1;

        // Check if the capture is a pawn promotion
        const std::int32_t promotionRank = (color == WHITE) ? 7 : 0;
        if ((targetSquare / 8) == promotionRank) {
            legalMoves.emplace_back(sourceSquare, targetSquare, KNIGHT_PROMOTE_CAPTURE, color, KNIGHT);
            legalMoves.emplace_back(sourceSquare, targetSquare, BISHOP_PROMOTE_CAPTURE, color, BISHOP);
            legalMoves.emplace_back(sourceSquare, targetSquare, ROOK_PROMOTE_CAPTURE, color, ROOK);
            legalMoves.emplace_back(sourceSquare, targetSquare, QUEEN_PROMOTE_CAPTURE, color, QUEEN);
        }
        else {
            legalMoves.emplace_back(sourceSquare, targetSquare, CAPTURE, color, PAWN);
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
                legalMoves.emplace_back(leftCaptureSquare, enPassantSquare, EN_PASSANT_CAPTURE, color, PAWN);
            }
        }

        if (enPassantFile < 7 && rightCaptureSquare >= 0 && rightCaptureSquare < 64) {
            const std::uint64_t enPassantRightCaptureMask = 1ULL << rightCaptureSquare;
            if (pawns & enPassantRightCaptureMask) {
                legalMoves.emplace_back(rightCaptureSquare, enPassantSquare, EN_PASSANT_CAPTURE, color, PAWN);
            }
        }
    }

}


inline void Board::generateNonSlidingMoves(const std::int32_t square, std::vector<Move>& legalMoves, std::uint64_t mask, Piece piece) const {
    const std::uint64_t pieces = (whitePieces & (1ULL << square)) ? whitePieces : blackPieces;
    const std::uint64_t oppsitePieces = (whitePieces & (1ULL << square)) ? blackPieces : whitePieces;

    std::uint64_t captureMoves = (mask & ~pieces) & oppsitePieces;
    std::uint64_t nonCaptureMoves = (mask & ~pieces) & ~oppsitePieces;

    while (captureMoves != 0) {
        const std::uint32_t target = getLSB(captureMoves);
        const Color targetColor = (whitePieces & (1ULL << square)) ? WHITE : BLACK;
        legalMoves.emplace_back(square, target, CAPTURE, targetColor, piece);
        captureMoves &= ~(1ULL << target);
    }

    while (nonCaptureMoves != 0) {
        const std::uint32_t target = getLSB(nonCaptureMoves);
        const Color targetColor = (whitePieces & (1ULL << square)) ? WHITE : BLACK;
        legalMoves.emplace_back(square, target, QUIET_MOVE, targetColor, piece);
        nonCaptureMoves &= ~(1ULL << target);
    }
}

void Board::generateSlidingMoves(std::int32_t square, std::vector<Move>& legalMoves, std::uint64_t mask,std::uint64_t magic_number, std::uint64_t attacks[4096], Piece piece) const {
    std::uint64_t occupancy = ((whitePieces | blackPieces) & mask); 

    int index = (int)((occupancy*magic_number) >> (64 - __popcnt64(mask)));

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
        legalMoves.emplace_back(square, target, CAPTURE, targetColor, piece);
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
    int index = (int)((occupancy * magic_number) >> (64 - __popcnt64(mask)));
    std::uint64_t attack = attacks[index];

    std::uint64_t moves = attack & ~(((whitePieces & (1ULL << square)) ?  whitePieces : blackPieces));

    return moves;
}


std::uint64_t Board::generatePawnMovesAsBits(const Color color) const {
    const std::uint64_t pawns = (color == WHITE) ? whitePawns : blackPawns;
    const std::uint64_t oppsitePieces = (color == WHITE) ? blackPieces : whitePieces;

    // Determine the direction based on the pawn color
    const std::int32_t direction = (color == WHITE) ? 1 : -1;

    // Generate pawn captures
    std::uint64_t pawnLeftCaptures = ((((color == WHITE) ? (pawns << 8) : (pawns >> 8)) >> 1) & ~(0x8080808080808080ULL)) & oppsitePieces;
    std::uint64_t pawnRightCaptures = ((((color == WHITE) ? (pawns << 8) : (pawns >> 8)) << 1) & ~(0x0101010101010101ULL)) & oppsitePieces;


    // Combine left and right captures
    std::uint64_t captures = pawnLeftCaptures | pawnRightCaptures;

    return captures;
}



std::uint64_t Board::GetAttackedPieces(Color color) const {
    std::uint64_t attackedPieces = 0;

    attackedPieces |= generatePawnMovesAsBits(color);  // Modify this function to update the attackedPieces bitboard instead of the legalMoves vector

    std::uint64_t knights = color == WHITE ? whiteKnights : blackKnights;

    while (knights) {
        const int square = getLSB(knights);

        attackedPieces |= generateNonSlidingMovesAsBits(square, Bitboard::knightAttack[square], KNIGHT);

        // Clear the least significant bit of the current piece
        knights &= (knights - 1);
    }

    std::uint64_t kings = color == WHITE ? whiteKing : blackKing;

    while (kings) {
        const int square = getLSB(kings);

        attackedPieces |= generateNonSlidingMovesAsBits(square, Bitboard::kingAttack[square], KING);

        // Clear the least significant bit of the current piece
        kings &= (kings - 1);
    }

    std::uint64_t bishops = color == WHITE ? whiteBishops : blackBishops;

    while (bishops) {
        const int square = getLSB(bishops);

        attackedPieces |= generateSlidingMovesAsBits(square, Bitboard::bishopMasks[square], Bitboard::bishopMagic[square], Bitboard::bishopAttacks[square], BISHOP);

        // Clear the least significant bit of the current piece
        bishops &= (bishops - 1);
    }

    std::uint64_t rooks = color == WHITE ? whiteRooks : blackRooks;

    while (rooks) {
        const int square = getLSB(rooks);

        attackedPieces |= generateSlidingMovesAsBits(square, Bitboard::rookMasks[square], Bitboard::rookMagic[square], Bitboard::rookAttacks[square], ROOK);

        // Clear the least significant bit of the current piece
        rooks &= (rooks - 1);
    }

    std::uint64_t queens = color == WHITE ? whiteQueens : blackQueens;

    while (queens) {
        const int square = getLSB(queens);

        attackedPieces |= generateSlidingMovesAsBits(square, Bitboard::rookMasks[square], Bitboard::rookMagic[square], Bitboard::rookAttacks[square], QUEEN);
        attackedPieces |= generateSlidingMovesAsBits(square, Bitboard::bishopMasks[square], Bitboard::bishopMagic[square], Bitboard::bishopAttacks[square], QUEEN);

        // Clear the least significant bit of the current piece
        queens &= (queens - 1);
    }

    return attackedPieces;
}
