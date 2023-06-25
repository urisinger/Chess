#include "magic bitboard/misc.h"
#ifdef WIN32
#include <Windows.h>
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
