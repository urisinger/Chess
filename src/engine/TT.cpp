#include "engine/TT.h"

void TTable::WriteHash(uint64_t key, int score, int depth, Move bestMove, HashFlags flag, int ply) {
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


bool TTable::ProbeHash(uint64_t key, THash* hashEntry, int ply) {
	*hashEntry = hashTable[key % size];

	if (hashEntry->score > MATE_SCORE)
		hashEntry->score -= ply;
	else if (hashEntry->score < -MATE_SCORE)
		hashEntry->score += ply;

	return hashEntry->key == key;
}


