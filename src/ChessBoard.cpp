#include "ChessBoard.h"



ChessBoard::ChessBoard(const std::string& FEN) : Board(FEN) {
    parseFEN(FEN);
}

ChessBoard::~ChessBoard() {
}

void ChessBoard::parseFEN(const std::string& FEN) {

    // Parse the side to move field
    const std::string& sideToMove = FEN.substr(FEN.find(' ') + 1, 1);
    WhiteToMove = (sideToMove == "w");

    // Parse the castling rights field
    const std::string& castlingRights = FEN.substr(FEN.find(' ') + 3, 4);
    CastlingRights = 0;
    for (char c : castlingRights) {
        switch (c) {
        case 'K': CastlingRights |= WHITE_KINGSIDE_CASTLING; break;
        case 'Q': CastlingRights |= WHITE_QUEENSIDE_CASTLING; break;
        case 'k': CastlingRights |= BLACK_KINGSIDE_CASTLING; break;
        case 'q': CastlingRights |= BLACK_QUEENSIDE_CASTLING; break;
        default: break;
        }
    }

    // Parse the en passant target square field
    const std::string& enPassantTarget = FEN.substr(FEN.find(' ') + 8, 2);
    if (enPassantTarget != "-") {
        // Convert the algebraic notation to square index
        int file = enPassantTarget[0] - 'a';
        int rank = 8 - (enPassantTarget[1] - '0');
        EnPassantTargetSquare = rank * 8 + file;
    }
    else {
        EnPassantTargetSquare = -1; // No en passant target square
    }

    // Parse the halfmove clock field
    const std::string& halfmoveClock = FEN.substr(FEN.find(' ') + 11, 1);
    HalfmoveClock = std::stoi(halfmoveClock);

    // Parse the fullmove number field
    const std::string& fullmoveNumber = FEN.substr(FEN.find(' ') + 13);
    FullmoveNumber = std::stoi(fullmoveNumber);
}




void ChessBoard::MakeMove(const Move& move) {
    int from = move.from;
    int to = move.to;

    // Check if the 'from' square is empty or out of bounds
    if (!Board.isSquareSet(from))
        return;

    // Get the piece on the 'from' square
    auto piece = Board.getSqaure(from);

    // Handle different types of moves using a switch statement
    switch (piece.first) {
    case PAWN:
        MovePawn(move, piece.second);
        break;

    case ROOK:
        MoveRook(move, piece.second);
        break;

    case KNIGHT:
        MoveKnight(move, piece.second);
        break;

    case BISHOP:
        MoveBishop(move, piece.second);
        break;

    case QUEEN:
        MoveQueen(move, piece.second);
        break;

    case KING:
        MoveKing(move, piece.second);
        break;

    default:
        return;
    }
}




std::vector<char> ChessBoard::findLegalMoves(char square) {
    std::vector<char> availableSquares;
    std::pair<Piece, Color> piece = Board.GetPiece(square);

    // Check if the square is occupied by a piece
    if (piece.first == EMPTY) {
        return availableSquares;  // Empty square has no legal moves
    }

    // Check for legal moves based on the piece type
    switch (piece.first) {
    case PAWN:
        availableSquares = Board.GeneratePawnMoves(square, piece.second);
        break;
    case BISHOP:
        availableSquares = Board.GenerateBishopMoves(square);
        break;
    case KING:
        availableSquares = Board.GenerateKingMoves(square);
        break;
    case KNIGHT:
        availableSquares = Board.GenerateKnightMoves(square);
        break;
    case ROOK:
        availableSquares = Board.GenerateRookMoves(square);
        break;
    case QUEEN:
        availableSquares = Board.GenerateQueenMoves(square);
        break;
    }

    // Check move legality
    std::vector<char> legalMoves;
    for (char targetSquare : availableSquares) {
        Move move{ square, targetSquare };
        if (IsMoveLegal(move)) {
            legalMoves.push_back(targetSquare);
        }
    }

    return legalMoves;
}



char ChessBoard::GetKingPosition(bool isWhite) const {
    for (int square = 0; square < 64; ++square) {
        Piece piece = Board[square];
        if (piece == (isWhite ? WHITE_KING : BLACK_KING)) {
            return static_cast<char>(square);
        }
    }
    return -1; // King not found (invalid position)
}


bool ChessBoard::IsSquareAttacked(char square, bool attackerIsWhite) const {
    // Check for attacks from pawns
    char pawnDirection = (attackerIsWhite) ? -1 : 1;
    char pawnAttackerRow = square / 8 + pawnDirection;
    char pawnAttackerColLeft = (square % 8) - 1;
    char pawnAttackerColRight = (square % 8) + 1;

    if (pawnAttackerRow >= 0 && pawnAttackerRow < 8) {
        if (pawnAttackerColLeft >= 0 && pawnAttackerColLeft < 8) {
            char attackerSquare = pawnAttackerRow * 8 + pawnAttackerColLeft;
            Piece attackerPiece = Board[attackerSquare];

            if (attackerPiece == (attackerIsWhite ? WHITE_PAWN : BLACK_PAWN)) {
                return true;
            }
        }

        if (pawnAttackerColRight >= 0 && pawnAttackerColRight < 8) {
            char attackerSquare = pawnAttackerRow * 8 + pawnAttackerColRight;
            Piece attackerPiece = Board[attackerSquare];

            if (attackerPiece == (attackerIsWhite ? WHITE_PAWN : BLACK_PAWN)) {
                return true;
            }
        }
    }

    // Check for attacks from knights
    int knightDeltas[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };

    for (int delta : knightDeltas) {
        int attackerSquare = square + delta;
        int attackerRow = attackerSquare / 8;
        int attackerCol = attackerSquare % 8;

        if (attackerRow >= 0 && attackerRow < 8 && attackerCol >= 0 && attackerCol < 8) {
            Piece attackerPiece = Board[attackerSquare];

            if (attackerPiece == (attackerIsWhite ? WHITE_KNIGHT : BLACK_KNIGHT)) {
                return true;
            }
        }
    }

    // Check for attacks from bishops and queens (diagonal directions)
    for (int rowDirection : { -1, 1 }) {
        for (int colDirection : { -1, 1 }) {
            for (int i = 1; i <= 7; i++) {
                int attackerRow = square / 8 + (rowDirection * i);
                int attackerCol = square % 8 + (colDirection * i);

                if (attackerRow < 0 || attackerRow >= 8 || attackerCol < 0 || attackerCol >= 8)
                    break;

                int attackerSquare = attackerRow * 8 + attackerCol;
                Piece attackerPiece = Board[attackerSquare];

                if (attackerPiece != EMPTY) {
                    if (attackerPiece == (attackerIsWhite ? WHITE_BISHOP : BLACK_BISHOP) ||
                        attackerPiece == (attackerIsWhite ? WHITE_QUEEN : BLACK_QUEEN)) {
                        return true;
                    }
                    break;  // Stop searching in this direction
                }
            }
        }
    }

    // Check for attacks from rooks and queens (orthogonal directions)
    for (int rowDirection : { -1, 0, 1 }) {
        for (int colDirection : { -1, 0, 1 }) {
            if (rowDirection == 0 && colDirection == 0) {
                continue;  // Skip the current iteration, as it represents the current square itself
            }

            for (int i = 1; i <= 7; i++) {
                int attackerRow = square / 8 + (rowDirection * i);
                int attackerCol = square % 8 + (colDirection * i);

                if (attackerRow < 0 || attackerRow >= 8 || attackerCol < 0 || attackerCol >= 8)
                    break;

                int attackerSquare = attackerRow * 8 + attackerCol;
                Piece attackerPiece = Board[attackerSquare];

                if (attackerPiece != EMPTY) {
                    if (attackerPiece == (attackerIsWhite ? WHITE_ROOK : BLACK_ROOK) ||
                        attackerPiece == (attackerIsWhite ? WHITE_QUEEN : BLACK_QUEEN)) {
                        return true;
                    }
                    break;  // Stop searching in this direction
                }
            }
        }
    }

    return false;
}



bool ChessBoard::IsMoveLegal(const Move& move) {
    // Save the current board state
    ChessBoard savedState = *this;

    // Apply the move to the board
    MakeMove(move);

    // Check if the current player's king is in check
    char kingPosition = GetKingPosition(!WhiteToMove);
    bool isLegal = !IsSquareAttacked(kingPosition, WhiteToMove);

    // Restore the board state
    *this = savedState;

    return isLegal;
}



std::vector<char> ChessBoard::GeneratePawnMoveSquares(char square) {
    std::vector<char> availableSquares;
    Piece piece = Board[square];
    bool isWhite = (piece > 0);

    if (isWhite != WhiteToMove) {
        return availableSquares;
    }
    // Determine the pawn's starting rank and direction
    char startRank = isWhite ? 1 : 6;
    char direction = isWhite ? 1 : -1;

    // Move forward one square
    if (Board[square + direction * 8] == EMPTY) {
        availableSquares.push_back(square + direction * 8);

        // Move forward two squares from the starting rank
        if (square / 8 == startRank && Board[square + direction * 16] == EMPTY) {
            availableSquares.push_back(square + direction * 16);
        }
    }

    // Capture diagonally to the left
    if (square % 8 > 0) {
        int captureSquare = square + direction * 8 - 1;
        if (Board[captureSquare] != EMPTY && ((isWhite && Board[captureSquare] < 0) || (!isWhite && Board[captureSquare] > 0))) {
            availableSquares.push_back(captureSquare);
        }
    }

    // Capture diagonally to the right
    if (square % 8 < 7) {
        int captureSquare = square + direction * 8 + 1;
        if (Board[captureSquare] != EMPTY && ((isWhite && Board[captureSquare] < 0) || (!isWhite && Board[captureSquare] > 0))) {
            availableSquares.push_back(captureSquare);
        }
    }

    // Check for en passant captures
    if (EnPassantTargetSquare >= 0) {
        // En passant capture to the left
        if (square % 8 > 0 && square + direction * 8 -1 == EnPassantTargetSquare) {
            availableSquares.push_back(square + direction * 8 -1);
        }

        // En passant capture to the right
        if (square % 8 < 7 && square + direction * 8 +1 == EnPassantTargetSquare) {
            availableSquares.push_back(square + direction * 8 +1);
        }
    }

    return availableSquares;
}

void ChessBoard::MovePawn(const Move& move) {
    int from = move.from;
    int to = move.to;

    // Update the board
    Board[to] = Board[from];
    Board[from] = EMPTY;


    // Handle en passant capture
    if (to == EnPassantTargetSquare) {
        // Determine the captured pawn's square
        int captureSquare = WhiteToMove ? to - 8 : to + 8;
        // Clear the captured pawn's square on the board
        Board[captureSquare] = EMPTY;
    }


    // Update en passant target square
    if (std::abs(from - to) == 16) {
        EnPassantTargetSquare = WhiteToMove ? to - 8 : to + 8;  // Set the en passant target square for the next move
    }
    else {
        EnPassantTargetSquare = -1;  // Clear the en passant target square
    }

    // Update the halfmove clock
    if (Board[to] == EMPTY || Board[to] == WHITE_PAWN || Board[to] == BLACK_PAWN) {
        HalfmoveClock = 0;  // Reset the halfmove clock if a pawn move is made
    }
    else {
        HalfmoveClock++;  // Increment the halfmove clock for non-pawn moves
    }

    // Update the fullmove number
    if (WhiteToMove) {
        FullmoveNumber++;  // Increment the fullmove number after Black's move
    }

    // Switch player turn
    WhiteToMove = !WhiteToMove;
}


void ChessBoard::MoveKing(const Move& move) {
    int from = move.from;
    int to = move.to;

    // Calculate the distance moved by the king
    int distance = std::abs(to - from);

    // Clear castling rights for the moved king
    if (WhiteToMove) {
        CastlingRights &= ~WHITE_KINGSIDE_CASTLING;
        CastlingRights &= ~WHITE_QUEENSIDE_CASTLING;
    }
    else {
        CastlingRights &= ~BLACK_KINGSIDE_CASTLING;
        CastlingRights &= ~BLACK_QUEENSIDE_CASTLING;
    }

    // Handle special moves
    if (distance == 2) {
        // Handle castling
        if (to == from + 2) {
            // King-side castle
            int rookFrom = from + 3;
            int rookTo = from + 1;
            Board[rookTo] = Board[rookFrom];
            Board[rookFrom] = EMPTY;
        }
        else if (to == from - 2) {
            // Queen-side castle
            int rookFrom = from - 4;
            int rookTo = from - 1;
            Board[rookTo] = Board[rookFrom];
            Board[rookFrom] = EMPTY;
        }
    }

    // Update the board
    Board[to] = Board[from];
    Board[from] = EMPTY;

    // Update en passant target square
    EnPassantTargetSquare = -1;

    HalfmoveClock++;  // Increment the halfmove clock for non-pawn moves

    // Update the fullmove number
    if (WhiteToMove) {
        FullmoveNumber++;  // Increment the fullmove number after Black's move
    }

    // Switch player turn
    WhiteToMove = !WhiteToMove;
}


std::vector<char> ChessBoard::GenerateKingMoves(char square) {
    std::vector<char> availableSquares;
    Piece piece = Board[square];
    bool isWhite = (piece > 0);

    if (isWhite != WhiteToMove) {
        return availableSquares;
    }

    // Determine the king's current rank and file
    char rank = square / 8;
    char file = square % 8;

    // Check all possible directions around the king
    for (int dr = -1; dr <= 1; dr++) {
        for (int df = -1; df <= 1; df++) {
            // Skip the current square
            if (dr == 0 && df == 0) {
                continue;
            }

            // Calculate the target square
            char targetRank = rank + dr;
            char targetFile = file + df;

            // Check if the target square is valid
            if (targetRank >= 0 && targetRank < 8 && targetFile >= 0 && targetFile < 8) {
                char targetSquare = targetRank * 8 + targetFile;
                Piece targetPiece = Board[targetSquare];

                // Determine if the target square is empty or contains an opponent's piece
                if (targetPiece == EMPTY || (isWhite && targetPiece < 0) || (!isWhite && targetPiece > 0)) {
                    availableSquares.push_back(targetSquare);
                }
            }
        }
    }

    // Check for castling moves
    if (isWhite && rank == 0 && file == 4) {
        if ((CastlingRights & WHITE_KINGSIDE_CASTLING) && Board[5] == EMPTY && Board[6] == EMPTY) {
            availableSquares.push_back(6);  // Add king-side castle move
        }
        if ((CastlingRights & WHITE_QUEENSIDE_CASTLING) && Board[3] == EMPTY && Board[2] == EMPTY && Board[1] == EMPTY) {
            availableSquares.push_back(2);  // Add queen-side castle move
        }
    }
    else if (!isWhite && rank == 7 && file == 4) {
        if ((CastlingRights & BLACK_KINGSIDE_CASTLING) && Board[61] == EMPTY && Board[62] == EMPTY) {
            availableSquares.push_back(62);  // Add king-side castle move
        }
        if ((CastlingRights & BLACK_QUEENSIDE_CASTLING) && Board[59] == EMPTY && Board[58] == EMPTY && Board[57] == EMPTY) {
            availableSquares.push_back(58);  // Add queen-side castle move
        }
    }

    return availableSquares;
}



void ChessBoard::MoveBishop(const Move& move) {
    int from = move.from;
    int to = move.to;

    // Update the board
    Board[to] = Board[from];
    Board[from] = EMPTY;

    // Update the en passant target square
    EnPassantTargetSquare = -1;

    // Update the halfmove clock
    HalfmoveClock++;

    // Update the fullmove number
    if (WhiteToMove) {
        FullmoveNumber++;
    }

    // Switch player turn
    WhiteToMove = !WhiteToMove;
}

std::vector<char> ChessBoard::GenerateBishopMoves(char square) {
    std::vector<char> availableSquares;
    Piece piece = Board[square];
    bool isWhite = (piece > 0);


    if (isWhite != WhiteToMove) {
        return availableSquares;
    }

    // Determine the bishop's current rank and file
    char rank = square / 8;
    char file = square % 8;

    // Check all diagonal directions
    for (int dr = -1; dr <= 1; dr += 2) {
        for (int df = -1; df <= 1; df += 2) {
            // Check along the diagonal until reaching the board edge or another piece
            for (int step = 1; step < 8; step++) {
                // Calculate the target square
                char targetRank = rank + dr * step;
                char targetFile = file + df * step;

                // Check if the target square is valid
                if (targetRank >= 0 && targetRank < 8 && targetFile >= 0 && targetFile < 8) {
                    char targetSquare = targetRank * 8 + targetFile;
                    Piece targetPiece = Board[targetSquare];

                    // Determine if the target square is empty or contains an opponent's piece
                    if (targetPiece == EMPTY || (isWhite && targetPiece < 0) || (!isWhite && targetPiece > 0)) {
                        availableSquares.push_back(targetSquare);

                        // Break if a piece is encountered (capturing or blocking)
                        if (targetPiece != EMPTY) {
                            break;
                        }
                    }
                    else {
                        break;  // Break if own piece is encountered (blocking)
                    }
                }
                else {
                    break;  // Break if outside the board
                }
            }
        }
    }

    return availableSquares;
}


void ChessBoard::MoveKnight(const Move& move) {
    int from = move.from;
    int to = move.to;

    // Update the board
    Board[to] = Board[from];
    Board[from] = EMPTY;

    // Update the en passant target square
    EnPassantTargetSquare = -1;

    // Update the halfmove clock
    HalfmoveClock++;

    // Update the fullmove number
    if (WhiteToMove) {
        FullmoveNumber++;
    }

    // Switch player turn
    WhiteToMove = !WhiteToMove;
}

std::vector<char> ChessBoard::GenerateKnightMoves(char square) {
    std::vector<char> availableSquares;
    Piece piece = Board[square];
    bool isWhite = (piece > 0);

    if (isWhite != WhiteToMove) {
        return availableSquares;
    }

    // Determine the knight's current rank and file
    char rank = square / 8;
    char file = square % 8;

    // Define the possible knight moves relative to its position
    const int knightMoves[8][2] = { {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1} };

    // Generate knight moves
    for (const auto& move : knightMoves) {
        int targetRank = rank + move[0];
        int targetFile = file + move[1];

        // Check if the target square is valid
        if (targetRank >= 0 && targetRank < 8 && targetFile >= 0 && targetFile < 8) {
            int targetSquare = targetRank * 8 + targetFile;
            Piece targetPiece = Board[targetSquare];

            // Determine if the target square is empty or contains an opponent's piece
            if (targetPiece == EMPTY || (isWhite && targetPiece < 0) || (!isWhite && targetPiece > 0)) {
                availableSquares.push_back(targetSquare);
            }
        }
    }

    return availableSquares;
}


std::vector<char> ChessBoard::GenerateRookMoves(char square) {
    std::vector<char> availableSquares;
    Piece piece = Board[square];
    bool isWhite = (piece > 0);

    if (isWhite != WhiteToMove) {
        return availableSquares;
    }

    // Determine the rook's current rank and file
    char rank = square / 8;
    char file = square % 8;

    // Define the possible relative rook moves
    const int rookMoves[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    // Generate rook moves
    for (const auto& move : rookMoves) {
        int dr = move[0];
        int df = move[1];

        // Check each square in the current direction
        for (int step = 1;; step++) {
            // Calculate the target square
            int targetRank = rank + step * dr;
            int targetFile = file + step * df;

            // Check if the target square is valid
            if (targetRank < 0 || targetRank >= 8 || targetFile < 0 || targetFile >= 8) {
                break;  // Exit the loop if the target square is out of bounds
            }

            int targetSquare = targetRank * 8 + targetFile;
            Piece targetPiece = Board[targetSquare];

            // Determine if the target square is empty or contains an opponent's piece
            if (targetPiece == EMPTY) {
                availableSquares.push_back(targetSquare);
            }
            else {
                if ((isWhite && targetPiece < 0) || (!isWhite && targetPiece > 0)) {
                    availableSquares.push_back(targetSquare);
                }
                break;  // Exit the loop if the target square contains a piece
            }
        }
    }

    return availableSquares;
}


void ChessBoard::MoveRook(const Move& move) {
    int from = move.from;
    int to = move.to;

    // Update the board
    Board[to] = Board[from];
    Board[from] = EMPTY;

    // Update the castle flags
    if (from == 0) {
        if (WhiteToMove)
            CastlingRights &= ~WHITE_QUEENSIDE_CASTLING;
        else
            CastlingRights &= ~BLACK_QUEENSIDE_CASTLING;
    }
    else if (from == 7) {
        if (WhiteToMove)
            CastlingRights &= ~WHITE_KINGSIDE_CASTLING;
        else
            CastlingRights &= ~BLACK_KINGSIDE_CASTLING;
    }
    else if (from == 56) {
        if (WhiteToMove)
            CastlingRights &= ~WHITE_QUEENSIDE_CASTLING;
        else
            CastlingRights &= ~BLACK_QUEENSIDE_CASTLING;
    }
    else if (from == 63) {
        if (WhiteToMove)
            CastlingRights &= ~WHITE_KINGSIDE_CASTLING;
        else
            CastlingRights &= ~BLACK_KINGSIDE_CASTLING;
    }

    // Update the halfmove clock
    HalfmoveClock++;

    // Update the fullmove number
    if (WhiteToMove) {
        FullmoveNumber++;
    }

    // Switch player turn
    WhiteToMove = !WhiteToMove;
}


void ChessBoard::MoveQueen(const Move& move) {
    int from = move.from;
    int to = move.to;

    // Update the board
    Board[to] = Board[from];
    Board[from] = EMPTY;

    // Update en passant target square
    EnPassantTargetSquare = -1;

    HalfmoveClock++;  // Increment the halfmove clock for non-pawn moves

    // Update the fullmove number
    if (WhiteToMove) {
        FullmoveNumber++;  // Increment the fullmove number after Black's move
    }

    // Switch player turn
    WhiteToMove = !WhiteToMove;
}

std::vector<char> ChessBoard::GenerateQueenMoves(char square) {
    std::vector<char> availableSquares;
    Piece piece = Board[square];
    bool isWhite = (piece > 0);

    if (isWhite != WhiteToMove) {
        return availableSquares;
    }

    // Determine the queen's current rank and file
    char rank = square / 8;
    char file = square % 8;

    // Check all possible directions for the queen: horizontal, vertical, and diagonal
    for (int dr = -1; dr <= 1; dr++) {
        for (int df = -1; df <= 1; df++) {
            // Skip the current square
            if (dr == 0 && df == 0) {
                continue;
            }

            // Check along the current direction until the edge of the board is reached or a piece is encountered
            char targetRank = rank + dr;
            char targetFile = file + df;
            while (targetRank >= 0 && targetRank < 8 && targetFile >= 0 && targetFile < 8) {
                char targetSquare = targetRank * 8 + targetFile;
                Piece targetPiece = Board[targetSquare];

                // If the target square is empty, add it to the available squares
                if (targetPiece == EMPTY) {
                    availableSquares.push_back(targetSquare);
                }
                // If the target square contains an opponent's piece, add it to the available squares and stop checking along this direction
                else if ((isWhite && targetPiece < 0) || (!isWhite && targetPiece > 0)) {
                    availableSquares.push_back(targetSquare);
                    break;
                }
                // If the target square contains a friendly piece, stop checking along this direction
                else {
                    break;
                }

                // Move to the next square along the current direction
                targetRank += dr;
                targetFile += df;
            }
        }
    }

    return availableSquares;
}
