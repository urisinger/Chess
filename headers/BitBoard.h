#include <cstdint>
#include <string>
#include <sstream>
#include <cctype>
#include <vector>

#include "BitMasks.h"



constexpr uint8_t WHITE_KINGSIDE_CASTLING = 0x1;      // 00000001
constexpr uint8_t WHITE_QUEENSIDE_CASTLING = 0x2;     // 00000010
constexpr uint8_t BLACK_KINGSIDE_CASTLING = 0x4;      // 00000100
constexpr uint8_t BLACK_QUEENSIDE_CASTLING = 0x8;     // 00001000


enum Piece {
    EMPTY=0,
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
    KNIGHT_PROMOTE,
    BISHOP_PROMOTE,
    ROOK_PROMOTE,
    QUEEN_PROMOTE,
    KNIGHT_PROMOTE_CAPTURE,
    BISHOP_PROMOTE_CAPTURE,
    ROOK_PROMOTE_CAPTURE,
    QUEEN_PROMOTE_CAPTURE
};

enum Color {
    BLACK,
    WHITE
};

struct Move {
    Move(unsigned int from, unsigned int to, unsigned int flags, Color color, Piece piece) {
        m_Move = ((static_cast<unsigned int>(piece) & 0x7) << 20) | ((static_cast<unsigned int>(color) & 0x1) << 16) |
            ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
    }

    void operator=(Move a) { m_Move = a.m_Move; }

    inline unsigned int getTo() const { return m_Move & 0x3f; }
    inline unsigned int getFrom() const { return (m_Move >> 6) & 0x3f; }
    inline MoveFlag getFlags() const { return static_cast<MoveFlag>((m_Move >> 12) & 0x0f); }

    inline void setTo(unsigned int to) { m_Move &= ~0x3f; m_Move |= to & 0x3f; }
    inline void setFrom(unsigned int from) { m_Move &= ~0xfc0; m_Move |= (from & 0x3f) << 6; }

    inline unsigned int getButterflyIndex() const { return m_Move & 0x0fff; }

    inline Color getColor() const { return static_cast<Color>((m_Move >> 16) & 0x1); }
    inline Piece getPiece() const { return static_cast<Piece>((m_Move >> 20) & 0x7); }

    inline bool operator==(Move a) const { return (m_Move & 0xffff) == (a.m_Move & 0xffff); }
    inline bool operator!=(Move a) const { return (m_Move & 0xffff) != (a.m_Move & 0xffff); }

private:
    unsigned int m_Move;
};

class Board {
public:
    Board(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    void getBoard(int board[64]) ;

    bool isSquareOccupied(std::uint32_t square) const;
    Piece getPiece(std::uint32_t square) const;
    void clearSquare(std::uint32_t square);
    void setSquare(std::uint32_t square, Color color, Piece piece);

    void movePiece(const Move& move);

    std::vector<Move> Board::GenerateLegalMoves(Color color) const;

    Color currentPlayer;

private:
    std::uint64_t whitePawns;
    std::uint64_t whiteKnights;
    std::uint64_t whiteBishops;
    std::uint64_t whiteRooks;
    std::uint64_t whiteQueens;
    std::uint64_t whiteKing;
    std::uint64_t blackPawns;
    std::uint64_t blackKnights;
    std::uint64_t blackBishops;
    std::uint64_t blackRooks;
    std::uint64_t blackQueens;
    std::uint64_t blackKing;

    std::uint64_t blackPieces;
    std::uint64_t whitePieces;


    std::uint8_t castleFlags;

    std::uint8_t halfMoveClock;
    std::uint16_t fullMoveNumber;

    std::uint8_t enPassantSquare;




    std::uint64_t getColorPieces(Color color) const;

    std::uint64_t& Board::getWhitePiece(Piece piece);

    std::uint64_t& Board::getBlackPiece(Piece piece);

    bool isOccupiedByColor(std::int32_t square, Color color) const;

    std::uint64_t generateSlidingMovesAsBits(std::int32_t square, std::uint64_t mask, std::uint64_t magic_number, const std::uint64_t attacks[4096], Piece piece) const;
    std::uint64_t generateNonSlidingMovesAsBits(std::int32_t square, std::uint64_t mask, Piece piece) const;
    std::uint64_t generatePawnMovesAsBits(const Color color) const;

    std::uint64_t Board::GetAttackedPieces(Color color) const;


    inline void generatePawnMoves(const Color color, std::vector<Move>& legalMoves) const;
    inline void generateNonSlidingMoves(std::int32_t square, std::vector<Move>& legalMoves, std::uint64_t mask, Piece piece) const;
    void generateSlidingMoves(std::int32_t square, std::vector<Move>& legalMoves, std::uint64_t mask, std::uint64_t magic_number, std::uint64_t attacks[4096], Piece piece) const;


    Piece pieceFromChar(char c) const;
};

