#pragma once
#define MIN_SCORE -50000
#define MAX_SCORE 50000

#define MATE_VALUE 49000

#define MATE_SCORE 48000

#define max_repetition 300

#define max_ply 128

#define STOPPED 0

#define valWINDOW 50

#define HASH_SIZE 2000000

#define HASH_MAX 100


#include <cstdint>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <immintrin.h>

#ifdef _MSC_VER
#  include <intrin.h>
#  define __builtin_popcountll _mm_popcnt_u64
#  define __builtin_ia32_tzcnt_u64 _tzcnt_u64
#endif

int GetTimeMs();

unsigned long xorshf96(void);

uint64_t random_uint64();

int getLSB(uint64_t value);

int countBits(uint64_t value);