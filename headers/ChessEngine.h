#pragma once
#include "BitBoard.h"
#define MIN_SCORE -50000
#define MAX_SCORE 50000

#define MATE_VALUE 49000

#define MATE_SCORE 48000

#define max_ply 128

#define STOPPED 0

#define valWINDOW 50

#define NO_HASH_ENTRY 10234231

#define HASH_SIZE 2000000

#define HASH_MAX 100


int GetTimeMs();

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
	Move bestMove;
};

struct HashTable
{
	THash* hashTable;

	unsigned int size;

	HashTable(unsigned int _size) : size(_size), hashTable(0) {
		hashTable = new THash[size];
	}

	void Resize(unsigned int newSize) {
		delete[] hashTable;

		size = newSize;
		hashTable = new THash[size];
	}

	inline int ReadHash(std::uint64_t key, int alpha, int beta, Move* bestMove, int depth, int ply) {
		THash* hashEntry = &hashTable[key % size];


		if (hashEntry->key == key) {
			if (hashEntry->depth >= depth) {
				int score = hashEntry->score;
				if (score < -MATE_SCORE) {
					score += ply;
				}

				if (score > MATE_SCORE) {
					score -= ply;
				}

				if (hashEntry->flag == HASH_EXSACT) {
					return score;
				}

				if (hashEntry->flag == HASH_ALPHA && score <= alpha) {
					return alpha;
				}

				if (hashEntry->flag == HASH_BETA && score >= alpha) {
					return beta;
				}
			}
			*bestMove = hashEntry->bestMove;

		}
		return NO_HASH_ENTRY;
	}

	inline void WriteHash(std::uint64_t key, int score, int depth, Move bestMove, HashFlags flag, int ply) {
		THash* hashEntry = &hashTable[key % size];

		if (score < -MATE_SCORE) {
			score -= ply;
		}

		if (score > MATE_SCORE) {
			score += ply;
		}

		hashEntry->key = key;
		hashEntry->score = score;
		hashEntry->flag = flag;
		hashEntry->depth = depth;
		hashEntry->bestMove = bestMove;
	}
};




//memory is super unsafe(never fixing this)
class ChessEngine {
public:
	ChessEngine() : quit(false), Trasposition(HASH_SIZE), count(0), ply(0){ }

	Move BestMove(int maxDdepth, const Board& board);

	void RunPerftTest(int depth, const Board& board);

	int count;

	int offset;

	Move killer_moves[2][max_ply];

	int pv_length[max_ply];
	Move pv_table[max_ply][max_ply];

	int history_moves[12][64];

	HashTable Trasposition;

	bool quit;
	bool stop;
	int startTime;

	double maxTime;
	int ply;
private:

	int score_move(Move move);
	int compareMoves(Move moveA, Move moveB, Move bestMove);
	void swapMoves(Move& moveA, Move& moveB);
	int partitionMoves(Move* moves, int low, int high, Move bestMove);
	void quicksortMoves(Move* moves, int low, int high, Move bestMove);
	void sortMoves(LegalMoves& legalMoves, Move bestMove);


	int Perft(int depth, const Board& board);

	int NegMax(int depth, const Board& board, int alpha, int beta);
	int quiescence(const Board& board, int alpha, int beta);

	void communicate();
	void clearTables();
	const int reductionLimits = 3;
	const int fullDepthMoves = 4;
};



