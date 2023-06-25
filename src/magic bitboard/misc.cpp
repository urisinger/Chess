#include "magic bitboard/misc.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

int GetTimeMs() {
#ifdef WIN32
    return GetTickCount();
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
#endif
}

static unsigned long x = 47321, y = 36344069, z = 5241859;

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

uint64_t random_uint64() {
    uint64_t u1, u2, u3, u4;
    u1 = (uint64_t)((xorshf96())) & 0xFFFF; u2 = (uint64_t)((xorshf96())) & 0xFFFF;
    u3 = (uint64_t)((xorshf96())) & 0xFFFF; u4 = (uint64_t)((xorshf96())) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

int getLSB(uint64_t value) {
    if (value == 0) {
        return -1;  // No bits are set
    }

    return (int)__builtin_ia32_tzcnt_u64(value);
}

int countBits(uint64_t value){
    return (int)__builtin_popcountll(value);
}