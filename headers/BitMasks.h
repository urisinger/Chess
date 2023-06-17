#pragma once
#include <iostream>
#include <cstdint>

int countBits(std::uint64_t);
int getLSB(std::uint64_t value);
void print_bitboard(std::uint64_t bitboard);

class Bitboard {
public:
    static std::uint64_t bishopMasks[64];
    static std::uint64_t rookMasks[64];
    static std::uint64_t queenAttack[64];
    static std::uint64_t knightAttack[64];
    static std::uint64_t kingAttack[64];

    static std::uint64_t bishopMagic[64];
    static std::uint64_t rookMagic[64];

    static std::uint64_t bishopAttacks[64][512];
    static std::uint64_t rookAttacks[64][4096];

    static std::uint64_t pieceKeys[12][64];
    static std::uint64_t enPeasentKeys[64];
    static std::uint64_t CastleKeys[16];

    static std::uint64_t SideKey;



    static void initBitmasks() {
        generateBishopBitmasks();
        generateRookBitmasks();
        generateKnightBitmasks();
        generateKingBitmasks();

        generateBishopAttacks();
        generateRookAttacks();

        generateHashKeys(); 
    }

    static void generateHashKeys();

    static void generateBishopBitmasks();

    static void generateKingBitmasks();

    static void generateRookBitmasks();

    static void generateBishopAttacks();

    static void generateRookAttacks();


    static void generateQueenBitmasks();

    static void generateKnightBitmasks();

    static void generateBishopMagicNumbers();

    static void generateRookMagicNumbers();

private:
    static std::uint64_t getBishopAttacks(int square, std::uint64_t occupancy);
    static std::uint64_t getRookAttacks(int square, std::uint64_t occupancy);
    static std::uint64_t generateMagicNumber(int square, bool is_bishop);
};