#ifndef SLEEP_US_H
#define SLEEP_US_H

#include <stdint.h>
#include "getTickCount.h"

#ifdef _WIN32
#include <windows.h>
// sleep microseconds
inline void sleepus(DWORD us) {
    LARGE_INTEGER freq, start, now;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    do {
        QueryPerformanceCounter(&now);
    } while ((now.QuadPart - start.QuadPart) / ((double)freq.QuadPart) * 1000000.0 < us);
}

#else
#include <unistd.h>
// sleep microseconds
inline void sleepus(unsigned long ms) {
    usleep(ms);
}
#endif



inline void sleepusNonBlocking(unsigned long waitus, unsigned long* usWaitCounter, unsigned long* timeUSatLastCall, bool* sleeping)
{
    if (sleeping == false) {
        *timeUSatLastCall = getTickCountUS64();
    }
    if (*timeUSatLastCall == 0x0) {
        *timeUSatLastCall = getTickCountUS64();
    }
    *sleeping = true;
    uint64_t currentTimeUS = getTickCountUS64(); // US since sys start
    unsigned long timeDelta = currentTimeUS - *timeUSatLastCall;

    *usWaitCounter += timeDelta;
    *timeUSatLastCall = currentTimeUS;
    if (*usWaitCounter > waitus) {
        *sleeping = false;
        *usWaitCounter = 0;
        *timeUSatLastCall = 0x0;
    }
}


#endif // ! SLEEP_US_H
