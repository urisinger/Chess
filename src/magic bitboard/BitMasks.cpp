#include "magic bitboard/BitMasks.h"



/*
bitmask generators
*/

void Masks::generateKnightBitmasks() {
    constexpr std::int32_t knightOffsets[][2] = {
        { -1, -2 }, { 1, -2 },
        { -2, -1 }, { 2, -1 },
        { -2, 1 }, { 2, 1 },
        { -1, 2 }, { 1, 2 }
    };

    for (std::int32_t square = 0; square < 64; ++square) {
        uint64_t bitmask = 0;
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

void Masks::generateKingBitmasks() {
    constexpr std::int8_t kingOffsets[8][2] = {
        { -1, -1 }, { -1, 0 }, { -1, 1 },
        { 0, -1 },             { 0, 1 },
        { 1, -1 }, { 1, 0 }, { 1, 1 }
    };

    for (std::int8_t square = 0; square < 64; ++square) {
        uint64_t bitmask = 0;
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

void Masks::generateBishopBitmasks() {
    for (int square = 0; square < 64; ++square) {
        uint64_t bitmask = 0;
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

void Masks::generateRookBitmasks() {
    for (int square = 0; square < 64; ++square) {
        uint64_t bitmask = 0;
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



uint64_t Masks::getBishopAttacks(int square, uint64_t occupancy) {
    // result attacks Masks
    uint64_t attacks = 0ULL;

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

uint64_t setOccupancy(int square, int index, uint64_t mask) {
    // Set bits for each file
    uint64_t occupancy = 0;

    int bits = countBits(mask);
    for (int i = 0; i < bits; ++i) {
        int cur = getLSB(mask);
        mask &= ~(1ULL << cur);

        if (index & (1 << i)) {
            occupancy |= (1ULL << cur);
        }
    }
    return occupancy;
}

void Masks::generateBishopAttacks() {
    for (int square = 0; square < 64; ++square) {
        uint64_t mask = bishopMasks[square];
        int numbits = countBits(mask);
        int numOccupancies = 1 << numbits;


        for (int i = 0; i < numOccupancies; ++i) {
            uint64_t magic = (setOccupancy(square,i,mask) * bishopMagic[square]) >> (64 - numbits);
            bishopAttacks[square][magic] = getBishopAttacks(square, setOccupancy(square, i  , mask));
        }
    }
}

void Masks::generateBishopMagicNumbers() {
    for (int square = 0; square < 64; ++square) {
        bishopMagic[square] = generateMagicNumber(square, true);
    }
}






uint64_t Masks::getRookAttacks(int square, uint64_t occupancy) {
    // Result attacks Masks
    uint64_t attacks = 0ULL;

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

void Masks::generateRookAttacks() {

    for (int square = 0; square < 64; ++square) {
        uint64_t mask = rookMasks[square];
        int numbits = countBits(mask);
        int numOccupancies = 1 << numbits;


        for (int i = 0; i < numOccupancies; ++i) {
            uint64_t magic = (setOccupancy(square, i, mask) * rookMagic[square]) >> (64 - numbits);
            rookAttacks[square][magic] = getRookAttacks(square, setOccupancy(square, i, mask));
        }
    }
}

void Masks::generateRookMagicNumbers() {
    for (int square = 0; square < 64; ++square) {
        rookMagic[square] = generateMagicNumber(square, false);    
    }
}

void print_BitBoard(uint64_t BitBoards) {
    for (int i = 0; i < 64; i++) {
        std::cout << (int)((bool)((1ULL << i) & BitBoards)) << " ";
        if (i % 8 == 7)
            std::cout << std::endl;
    }
}


// find appropriate magic number
uint64_t Masks::generateMagicNumber(int square, bool is_bishop) {
    // init occupancies
    uint64_t occupancies[4096];

    // init attack tables
    uint64_t attacks[4096];

    // init used attacks
    uint64_t used_attacks[4096];

    // init attack mask for a current piece
    uint64_t attack_mask = is_bishop ? bishopMasks[square] : rookMasks[square];

    int relevant_bits = countBits(attack_mask);

    // init occupancy indicies
    int occupancy_indicies = 1 << relevant_bits;

    memset(occupancies,0ULL,sizeof(occupancies));

    for (int i = 0; i < occupancy_indicies; i++) {
        occupancies[i] = setOccupancy(square, i, attack_mask);
    }

    memset(attacks, 0ULL, sizeof(attacks));
    for (int i = 0; i < occupancy_indicies; i++) {
        attacks[i] = is_bishop ? getBishopAttacks(square, occupancies[i]) : getRookAttacks(square, occupancies[i]);
    }

    // test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++)
    {
        // generate magic number candidate
        uint64_t magic_number = random_uint64() & random_uint64() & random_uint64();

        // skip inappropriate magic numbers
        if (countBits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

        // init used attacks
        memset(used_attacks,0ULL,sizeof(used_attacks));


        // init index & fail flag
        int index, fail;

        // test magic index loop
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
        {
            // init magic index
            uint64_t magic_index = (occupancies[index] * magic_number) >> (64 - relevant_bits);

            // if magic index works
            if(used_attacks[magic_index] == 0ULL) {
                used_attacks[magic_index] = attacks[index];
            }
            else if(used_attacks[magic_index] != attacks[index]) {
                fail = 1;
            }
        }

        // if magic number works
        if (!fail) {
            return magic_number;
        }
    }

    // if magic number doesn't work
    printf("Magic number fails!\n");
    return 0ULL;
}


void Masks::generateHashKeys() {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            pieceKeys[i][j] = random_uint64();
        }

    }

    for (int i = 0; i < 64; i++) {
        enPeasentKeys[i] = random_uint64();
    }

    for (int i = 0; i < 16; i++) {
        CastleKeys[i] = random_uint64();
    }

    SideKey = random_uint64();
}


// Initialize static member variables 
uint64_t Masks::bishopMasks[64];
uint64_t Masks::rookMasks[64];
uint64_t Masks::queenAttack[64];
uint64_t Masks::knightAttack[64];
uint64_t Masks::kingAttack[64];

uint64_t Masks::bishopMagic[64] = 
{ 0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL };
uint64_t Masks::rookMagic[64] = 
{ 0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL } ;

uint64_t Masks::pieceKeys[12][64] = { 0 };
uint64_t Masks::enPeasentKeys[64] = { 0 };
uint64_t Masks::CastleKeys[16] = { 0 };

uint64_t Masks::SideKey = { 0 };

uint64_t Masks::bishopAttacks[64][512];
uint64_t Masks::rookAttacks[64][4096];