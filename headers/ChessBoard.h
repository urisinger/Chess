#pragma once
#include <string>
#include <sstream>
#include <vector>

#include "Board.h"


typedef struct Move {
    char from;  // Starting square index
    char to;    // Destination square index
}Move;

constexpr uint8_t WHITE_KINGSIDE_CASTLING = 0x1;      // 00000001
constexpr uint8_t WHITE_QUEENSIDE_CASTLING = 0x2;     // 00000010
constexpr uint8_t BLACK_KINGSIDE_CASTLING = 0x4;      // 00000100
constexpr uint8_t BLACK_QUEENSIDE_CASTLING = 0x8;     // 00001000

class ChessBoard {
public:
    ChessBoard(const std::string& FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ~ChessBoard();


    void MakeMove(const Move& move);
    std::vector<char> findLegalMoves(char square);

private:

    Board Board;
    bool WhiteToMove;
    uint8_t CastlingRights;
    char EnPassantTargetSquare;
    char HalfmoveClock;
    short FullmoveNumber;


    char GetKingPosition(bool iswhite) const;



    bool IsSquareAttacked(char square, bool attackerIsWhite) const;
    
    bool IsMoveLegal(const Move& move);


    void parseFEN(const std::string& FEN);

    void MovePawn(const Move& move);
    void MoveKing(const Move& move);
    void MoveBishop(const Move& move);
    void MoveKnight(const Move& move);
    void MoveRook(const Move& move);
    void MoveQueen(const Move& move);

    std::vector<char> GeneratePawnMoveSquares(char square);
    std::vector<char> GenerateRookMoves(char square);
    std::vector<char> GenerateKnightMoves(char square);
    std::vector<char> GenerateBishopMoves(char square);
    std::vector<char> GenerateQueenMoves(char square);
    std::vector<char> GenerateKingMoves(char square);

};