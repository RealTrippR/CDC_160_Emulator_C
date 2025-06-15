#ifndef SLEEPMS_H
#define SLEEPMS_H

#include <stdio.h>
#include <stdbool.h>
#include "getTickCount.h"

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


inline void sleepmsNonBlocking(unsigned long waitms, unsigned long* msWaitCounter, unsigned long* timeMSatLastCall, bool* sleeping)
{
    if (waitms == 0) {
        *sleeping = false;
        return;
    }
    if (*timeMSatLastCall == 0x0) {
        *timeMSatLastCall = getTickCount32();
    }
    *sleeping = true;
    unsigned long currentTimeMS = getTickCount32(); // milliseconds since system start
    unsigned long timeDelta = currentTimeMS - *timeMSatLastCall;

    *msWaitCounter += timeDelta;
    *timeMSatLastCall = currentTimeMS;
    if (*msWaitCounter > waitms) {
        *sleeping = false;
        *msWaitCounter = 0;
        *timeMSatLastCall = 0x0;
    }
}

#endif