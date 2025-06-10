#ifndef SLEEP_US_H
#define SLEEP_US_H

#include <stdint.h>

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


#endif // ! SLEEP_US_H
