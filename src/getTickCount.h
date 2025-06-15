#ifndef GET_TICK_COUNT_32_H
#define GET_TICK_COUNT_32_H
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
inline unsigned long getTickCount32() {
	return GetTickCount();
}

inline uint64_t getTickCount64() {
    return GetTickCount64();
}

inline uint64_t getTickCountUS64() {
    static LARGE_INTEGER frequency;
    static BOOL initialized = FALSE;
    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        initialized = TRUE;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000000) / frequency.QuadPart);
}
#else
#include <unistd.h>
#include <sys/time.h>

inline unsigned long getTickCount32() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned long)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

inline uint64_t getTickCount64() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

inline uint64_t getTickCountUS64() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}

#endif

#endif