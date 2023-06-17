#pragma once
#include "BitBoard.h"
#define MIN_SCORE -100000
#define MAX_SCORE 100000

#define max_ply 128

#define NO_HASH_ENTRY 10234231

#define HASH_SIZE 0x4000000	

enum HashFlags {
	HASH_EXSACT,
	HASH_ALPHA,
	HASH_BETA
};

struct THash {
	std::uint64_t key;
	int depth;
	HashFlags flag;
	int score;
};

struct HashTable
{
	THash* hashTable;

	size_t size;
	HashTable(size_t _size) : size(_size), hashTable(0){
		hashTable = new THash[size];
	}

	inline int ReadHash(std::uint64_t key, int alpha, int beta, int depth) {
		THash* hashEntry = &hashTable[key % size];

		if (hashEntry->key == key) {
			if (hashEntry->depth >= depth) {
				if (hashEntry->flag == HASH_EXSACT) {
					return hashEntry->score;
				}
				
				if (hashEntry->flag == HASH_ALPHA && hashEntry->score <= alpha) {
					return alpha;
				}

				if (hashEntry->flag == HASH_BETA && hashEntry->score >= alpha) {
					return beta;
				}
			}
		}
		return NO_HASH_ENTRY;
	}

	inline void WriteHash(std::uint64_t key, int score, int depth, HashFlags flag) {
		THash* hashEntry = &hashTable[key % size];

		hashEntry->key = key;
		hashEntry->score = score;
		hashEntry->flag = flag;
		hashEntry->depth = depth;
	}
};


//memory is super unsafe(never fixing this)
class ChessEngine {
public:
	ChessEngine(Board* board) { curBoard = board; count = 0; ply = 0; }

	int Minimax(int depth, const Board& board, int alpha, int beta);
	Move BestMove(double maxTime);
	int quiescence(const Board& board, int alpha, int beta);
	void setBoard(Board* board) { curBoard = board; }

	int Perft(int depth, const Board& board);
	void RunPerftTest(int depth);

	static int count;

	static Move killer_moves[2][max_ply];

	static int pv_length[max_ply];
	static Move pv_table[max_ply][max_ply];

	static int history_moves[12][64];

	static HashTable Trasposition;


	static int ply;
private:

	void clearTables();
	static const int reductionLimits;
	static const int fullDepthMoves;
	Board* curBoard;
};