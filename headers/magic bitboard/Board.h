#pragma once
#include <string>
#include <sstream>
#include <vector>

#include "BitMasks.h"



constexpr uint8_t WHITE_KINGSIDE_CASTLING = 0x1;      // 00000001
constexpr uint8_t WHITE_QUEENSIDE_CASTLING = 0x2;     // 00000010
constexpr uint8_t BLACK_KINGSIDE_CASTLING = 0x4;      // 00000100
constexpr uint8_t BLACK_QUEENSIDE_CASTLING = 0x8;     // 00001000




enum Piece {
    EMPTY = 0,
    QUEEN,
    KING,
    ROOK,
    KNIGHT,
    BISHOP,
    PAWN
};

enum MoveFlag {
    QUIET_MOVE = 0,
    DOUBLE_PUSH,
    KING_CASTLE,
    QUEEN_CASTLE,
    CAPTURE,
    EN_PASSANT_CAPTURE,
    CHECK,
    PROMOTE
};

enum Color {
    BLACK,
    WHITE
};

struct Move {
    Move() : m_Move(0) {
    }

    Move(unsigned int from, unsigned int to, unsigned int flags, Color color, Piece piece, Piece captured ) {
        m_Move = (((static_cast<unsigned int>(captured) & 0x7) << 23) | (static_cast<unsigned int>(piece) & 0x7) << 20) | ((static_cast<unsigned int>(color) & 0x1) << 16) |
            ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
    }

    void operator=(Move a) { m_Move = a.m_Move; }

    inline unsigned int getTo() const { return m_Move & 0x3f; }
    inline unsigned int getFrom() const { return (m_Move >> 6) & 0x3f; }
    inline MoveFlag getFlags() const { return static_cast<MoveFlag>((m_Move >> 12) & 0x0f); }
    inline void setFlags(MoveFlag flags) { m_Move &= ~0x0f000; m_Move |= (static_cast<unsigned int>(flags) & 0x0f) << 12; }


    inline void setTo(unsigned int to) { m_Move &= ~0x3f; m_Move |= to & 0x3f; }
    inline void setFrom(unsigned int from) { m_Move &= ~0xfc0; m_Move |= (from & 0x3f) << 6; }

    inline Piece getCapturedPiece() const { return static_cast<Piece>((m_Move >> 23) & 0x7); }

    inline unsigned int getButterflyIndex() const { return m_Move & 0x0fff; }

    inline Color getColor() const { return static_cast<Color>((m_Move >> 16) & 0x1); }
    inline Piece getPiece() const { return static_cast<Piece>((m_Move >> 20) & 0x7); }

    std::string to_str() const {
        char fromFile = 'a' + (getFrom() % 8);
        char toFile = 'a' + (getTo() % 8);
        std::stringstream ss;
        ss << fromFile << getFrom() / 8 + 1 << toFile << getTo() / 8 + 1;

        // Add promotion information if the move is a promotion
        if (getFlags() == MoveFlag::PROMOTE) {
            Piece promotedPiece = getPiece();

            switch (promotedPiece) {
            case Piece::QUEEN:
                ss << 'q';
                break;
            case Piece::ROOK:
                ss << 'r';
                break;
            case Piece::KNIGHT:
                ss << 'n';
                break;
            case Piece::BISHOP:
                ss << 'b';
                break;
            default:
                // Invalid or unexpected piece for promotion
                break;
            }
        }

        return ss.str();
    }


    inline bool operator==(Move a) const { return (m_Move & 0xffff) == (a.m_Move & 0xffff); }
    inline bool operator!=(Move a) const { return (m_Move & 0xffff) != (a.m_Move & 0xffff); }

private:
    unsigned int m_Move;
};

struct LegalMoves {
    int count;
    Move moves[256];

    LegalMoves() : count(0) {

    }

    void emplace_back(unsigned int from, unsigned int to, unsigned int flags, Color color, Piece piece, Piece captured = EMPTY) {
        moves[count] = Move(from, to, flags, color, piece,captured);
        count++;
    }

    void push_back(Move move) {
        moves[count] = move;
        count++;
    }

    void clear() {
        std::fill((unsigned  int*)moves, (unsigned  int*)(moves+256),0);
    }
};

class Board {
public:
    Board(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    void ParseFen(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    void getBoard(int board[64]);

    bool isSquareOccupied(uint32_t square) const;
    Piece getPiece(uint32_t square) const;
    void clearSquare(uint32_t square);
    void setSquare(uint32_t square, Color color, Piece piece);

    void MakeNullMove();
    void MakeMove(const Move& move);

    int SlowEval() const;

    void PrintBoard() const;


    LegalMoves GenerateLegalMoves(Color color) const;
    LegalMoves GenerateCaptureMoves(Color color) const;

    bool isSqaureAttacked(Color color, int square) const;
    bool isKingAttacked(Color color) const;

    uint64_t generateHashKey() const;

    Color currentPlayer;

    int eval;

    uint64_t hashKey;

    std::int8_t enPassantSquare;


private:
    uint64_t whitePawns;
    uint64_t whiteKnights;
    uint64_t whiteBishops;
    uint64_t whiteRooks;
    uint64_t whiteQueens;
    uint64_t whiteKing;
    uint64_t blackPawns;
    uint64_t blackKnights;
    uint64_t blackBishops;
    uint64_t blackRooks;
    uint64_t blackQueens;
    uint64_t blackKing;

    uint64_t blackPieces;
    uint64_t whitePieces;


    uint8_t castleFlags;

    uint8_t halfMoveClock;
    uint16_t fullMoveNumber;





    uint64_t getColorPieces(Color color) const;

    uint64_t& getWhitePiece(Piece piece);

    uint64_t& getBlackPiece(Piece piece);

    bool isOccupiedByColor(std::int32_t square, Color color) const;

    uint64_t generateSlidingMovesAsBits(std::int32_t square, uint64_t mask, uint64_t magic_number, const uint64_t attacks[4096], Piece piece) const;
    uint64_t generateNonSlidingMovesAsBits(std::int32_t square, uint64_t mask, Piece piece) const;
    uint64_t generatePawnMovesAsBits(const Color color) const;

    uint64_t GetAttackedPieces(Color color) const;


    inline void generatePawnMoves(const Color color, LegalMoves& legalMoves) const;
    inline void generateNonSlidingMoves(std::int32_t square, LegalMoves& legalMoves, uint64_t mask, Piece piece) const;
    inline void generateSlidingMoves(std::int32_t square, LegalMoves& legalMoves, uint64_t mask, uint64_t magic_number, uint64_t attacks[4096], Piece piece) const;

    inline void generatePawnAttacks(const Color color, LegalMoves& legalMoves) const;
    inline void generateNonSlidingAttacks(const std::int32_t square, LegalMoves& legalMoves, uint64_t mask, Piece piece) const;
    inline void generateSlidingAttacks(std::int32_t square, LegalMoves& legalMoves, uint64_t mask, uint64_t magic_number, uint64_t attacks[4096], Piece piece) const;



    Piece pieceFromChar(char c) const;
};
