#ifndef SLEEPMS_H
#define SLEEPMS_H

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
// sleep milliseconds
inline void sleepms(unsigned long ms) {
    Sleep(ms);
}
#else
#include <unistd.h>
// sleep milliseconds
inline void sleepms(unsigned long ms) {
    usleep(ms * 1000);
}
#endif

#endif