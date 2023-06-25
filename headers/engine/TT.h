#pragma once
#include "magic bitboard/Board.h"

enum HashFlags {
	HASH_EXSACT,
	HASH_ALPHA,
	HASH_BETA
};



struct THash {
	uint64_t key;
	int depth;
	HashFlags flag;
	int score;
	Move bestMove;
};

struct TTable
{
	THash* hashTable;

	unsigned int size;

	TTable(unsigned int _size) : size(_size), hashTable(0) {
		hashTable = new THash[size];
	}

	~TTable() {
		delete[] hashTable;
	}

	void Resize(unsigned int newSize) {
		delete[] hashTable;

		size = newSize;
		hashTable = new THash[size];
	}

	void clear() {
		std::fill((int*)(hashTable),(int*)(hashTable+size),0);
	}

	bool ProbeHash(uint64_t key, THash* hashEntry, int ply);



	void WriteHash(uint64_t key, int score, int depth, Move bestMove, HashFlags flag, int ply);
};
