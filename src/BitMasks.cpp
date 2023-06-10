#include "BitMasks.h"
#include <random>

/*
Help functions
*/
static unsigned long x = 123456789, y = 362436069, z = 521288629;

unsigned long xorshf96(void) {          //period 2^96-1
    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return z;
}

std::uint64_t random_uint64() {
    std::uint64_t u1, u2, u3, u4;
    u1 = (std::uint64_t)((xorshf96())) & 0xFFFF; u2 = (std::uint64_t)((xorshf96())) & 0xFFFF;
    u3 = (std::uint64_t)((xorshf96())) & 0xFFFF; u4 = (std::uint64_t)((xorshf96())) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

int getLSB(std::uint64_t value) {
    if (value == 0) {
        return -1;  // No bits are set
    }

    return (int)_tzcnt_u64(value);
}

/*
bitmask generators
*/

void Bitboard::generateQueenBitmasks() {
    for (int square = 0; square < 64; ++square) {
        queenAttack[square] = bishopMasks[square] | rookMasks[square];
    }
}

void Bitboard::generateKnightBitmasks() {
    constexpr std::int32_t knightOffsets[][2] = {
        { -1, -2 }, { 1, -2 },
        { -2, -1 }, { 2, -1 },
        { -2, 1 }, { 2, 1 },
        { -1, 2 }, { 1, 2 }
    };

    for (std::int32_t square = 0; square < 64; ++square) {
        std::uint64_t bitmask = 0;
        const std::int32_t rank = square / 8;
        const std::int32_t file = square % 8;

        for (const auto& offset : knightOffsets) {
            const std::int32_t targetRank = rank + offset[0];
            const std::int32_t targetFile = file + offset[1];

            if (targetRank >= 0 && targetRank < 8 && targetFile >= 0 && targetFile < 8) {
                const std::int32_t targetSquare = targetRank * 8 + targetFile;
                bitmask |= (1ULL << targetSquare);
            }
        }

        knightAttack[square] = bitmask;
    }
}

void Bitboard::generateKingBitmasks() {
    constexpr std::int8_t kingOffsets[8][2] = {
        { -1, -1 }, { -1, 0 }, { -1, 1 },
        { 0, -1 },             { 0, 1 },
        { 1, -1 }, { 1, 0 }, { 1, 1 }
    };

    for (std::int8_t square = 0; square < 64; ++square) {
        std::uint64_t bitmask = 0;
        std::int8_t rank = square / 8;
        std::int8_t file = square % 8;

        for (const auto& offset : kingOffsets) {
            std::int8_t targetRank = rank + offset[0];
            std::int8_t targetFile = file + offset[1];

            if (targetRank >= 0 && targetRank < 8 && targetFile >= 0 && targetFile < 8) {
                std::int8_t targetSquare = targetRank * 8 + targetFile;
                bitmask |= (1ULL << targetSquare);
            }
        }

        kingAttack[square] = bitmask;
    }
}

void Bitboard::generateBishopBitmasks() {
    for (int square = 0; square < 64; ++square) {
        std::uint64_t bitmask = 0;
        int rank = square / 8;
        int file = square % 8;

        for (int r = rank + 1, f = file + 1; r < 7 && f < 7; ++r, ++f) {
            bitmask |= (1ULL << (r * 8 + f));
        }

        for (int r = rank - 1, f = file + 1; r > 0 && f < 7; --r, ++f) {
            bitmask |= (1ULL << (r * 8 + f));
        }

        for (int r = rank + 1, f = file - 1; r < 7 && f > 0; ++r, --f) {
            bitmask |= (1ULL << (r * 8 + f));
        }

        for (int r = rank - 1, f = file - 1; r > 0 && f > 0; --r, --f) {
            bitmask |= (1ULL << (r * 8 + f));
        }

        bishopMasks[square] = bitmask;
    }
}

void Bitboard::generateRookBitmasks() {
    for (int square = 0; square < 64; ++square) {
        std::uint64_t bitmask = 0;
        int rank = square / 8;
        int file = square % 8;

        for (int r = rank + 1; r < 7; ++r) {
            bitmask |= (1ULL << (r * 8 + file));
        }

        for (int r = rank - 1; r > 0; --r) {
            bitmask |= (1ULL << (r * 8 + file));
        }

        for (int f = file + 1; f < 7; ++f) {
            bitmask |= (1ULL << (rank * 8 + f));
        }

        for (int f = file - 1; f > 0; --f) {
            bitmask |= (1ULL << (rank * 8 + f));
        }

        rookMasks[square] = bitmask;
    }
}



void Bitboard::generateBishopOccupancy(int square, std::uint64_t returns[]) {
    int rank = square / 8;
    int file = square % 8;

    std::uint64_t occupancy = 0;
    std::uint64_t mask = bishopMasks[square];
    int bits = __popcnt64(mask);

    // Generate occupancy masks for each possible occupancy set
    for (int i = 0; i < (1 << bits); ++i) {
        occupancy = 0;
        std::uint64_t mask2 = mask;

        // Set bits for each diagonal
        for (int j = 0; j < bits; ++j) {
            int cur = getLSB(mask2);
            mask2 &= ~(1ULL << cur);

            if (i & (1 << j)) {
                occupancy |= (1ULL << cur);
            }
        }

        returns[i] = occupancy;
    }

}

std::uint64_t Bitboard::getBishopAttacks(int square, std::uint64_t occupancy) {
    // result attacks bitboard
    std::uint64_t attacks = 0ULL;

    // init ranks & files
    int r, f;

    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;

    // generate bishop atacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy) break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy) break;
    }

    // return attack map
    return attacks;
}

void Bitboard::generateBishopAttacks() {
    for (int square = 0; square < 64; ++square) {
        std::uint64_t mask = bishopMasks[square];
        int numOccupancies = 1 << __popcnt64(mask);
        std::uint64_t occupancy[4096];

        generateBishopOccupancy(square, occupancy);

        for (int i = 0; i < numOccupancies; ++i) {
            bishopAttacks[square][i] = getBishopAttacks(square, occupancy[i]);
        }
    }
}

void Bitboard::generateBishopMagicNumbers() {
    for (int square = 0; square < 64; ++square) {
        bishopMagic[square] = generateMagicNumber(square, true);
    }
}



void Bitboard::generateRookOccupancy(int square, std::uint64_t returns[]) {
    int rank = square / 8;
    int file = square % 8;

    std::uint64_t occupancy = 0;
    std::uint64_t mask = rookMasks[square];
    int bits = __popcnt64(mask);

    // Generate occupancy masks for each possible occupancy set
    for (int i = (1 << bits)-1; i >= 0 ; --i) {
        occupancy = 0;
        std::uint64_t mask2 = mask;

        // Set bits for each file
        for (int j = bits-1; j >= 0; --j) {
            int cur = getLSB(mask2);
            mask2 &= ~(1ULL << cur);

            if (i & (1 << j)) {
                occupancy |= (1ULL << cur);
            }
        }

        returns[i] = occupancy;
    }
}


std::uint64_t Bitboard::getRookAttacks(int square, std::uint64_t occupancy) {
    // Result attacks bitboard
    std::uint64_t attacks = 0ULL;

    // Init ranks & files
    int r, f;

    // Init target rank & files
    int tr = square / 8;
    int tf = square % 8;

    // Generate rook attacks
    for (r = tr + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & occupancy) break;
    }

    for (r = tr - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & occupancy) break;
    }

    for (f = tf + 1; f <= 7; f++) {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & occupancy) break;
    }

    for (f = tf - 1; f >= 0; f--) {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & occupancy) break;
    }

    // Return attack map
    return attacks;
}

void Bitboard::generateRookAttacks() {
    for (int square = 0; square < 64; ++square) {
        std::uint64_t mask = rookMasks[square];
        int numOccupancies = 1 << __popcnt64(mask);
        std::uint64_t* attacks = rookAttacks[square];
        std::uint64_t occupancy[4096];

        generateRookOccupancy(square, occupancy);   

        for (int i = 0; i < numOccupancies; ++i) {
            attacks[i] = getRookAttacks(square, occupancy[i]);
        }
    }
}

void Bitboard::generateRookMagicNumbers() {
    for (int square = 0; square < 64; ++square) {
        rookMagic[square] = generateMagicNumber(square, false);    
    }
}

void print_bitboard(std::uint64_t bitboard) {
    for (int i = 0; i < 64; i++) {
        std::cout << (int)((bool)((1ULL << i) & bitboard)) << " ";
        if (i % 8 == 7)
            std::cout << std::endl;
    }
}


// find appropriate magic number
std::uint64_t Bitboard::generateMagicNumber(int square, bool is_bishop) {
    // init occupancies
    std::uint64_t occupancies[4096];

    // init attack tables
    std::uint64_t* attacks = is_bishop ? bishopAttacks[square] : rookAttacks[square];

    // init used attacks
    std::uint64_t used_attacks[4096];

    // init attack mask for a current piece
    std::uint64_t attack_mask = is_bishop ? bishopMasks[square] : rookMasks[square];

    int relevant_bits = __popcnt64(attack_mask);

    // init occupancy indicies
    int occupancy_indicies = 1 << relevant_bits;


    if (is_bishop) {
        generateBishopOccupancy(square, occupancies);
    }
    else {
        generateRookOccupancy(square, occupancies);
    }

    for(int i =0 ; i < (1 << relevant_bits); i++)
        attacks[i] = is_bishop ? getBishopAttacks(square, occupancies[i]) : getRookAttacks(square, occupancies[i]);


    // test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++)
    {
        // generate magic number candidate
        std::uint64_t magic_number = random_uint64() & random_uint64() & random_uint64();

        // skip inappropriate magic numbers
        if (__popcnt64((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

        // init used attacks
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        // init index & fail flag
        int index, fail;

        // test magic index loop
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
        {
            // init magic index
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));

            // if magic index works
            if (used_attacks[magic_index] == 0ULL) {
                // init used attacks
                used_attacks[magic_index] = attacks[index];
            }
            else if (used_attacks[magic_index] != attacks[index]) {
                fail = 1;
            }
        }

        // if magic number works
        if (!fail) {
            memcpy(attacks, used_attacks, sizeof(used_attacks));
            return magic_number;
        }
    }

    // if magic number doesn't work
    printf("  Magic number fails!\n");
    return 0ULL;
}




// Initialize static member variables 
std::uint64_t Bitboard::bishopMasks[64];
std::uint64_t Bitboard::rookMasks[64];
std::uint64_t Bitboard::queenAttack[64];
std::uint64_t Bitboard::knightAttack[64];
std::uint64_t Bitboard::kingAttack[64];

std::uint64_t Bitboard::bishopMagic[64];
std::uint64_t Bitboard::rookMagic[64];

std::uint64_t Bitboard::bishopAttacks[64][512];
std::uint64_t Bitboard::rookAttacks[64][4096];