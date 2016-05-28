#include "stdafx.h"


#include "WindowGameTimer.h"



WindowMasterGameTimer::WindowMasterGameTimer(void)
{
    mSupported = QueryPerformanceFrequency(&mFreq);
}

bool WindowMasterGameTimer::start(void)
{
    QueryPerformanceCounter(&mStart);
    return mSupported;
}

bool WindowMasterGameTimer::stop(void)
{
    return false;
}

unsigned long WindowMasterGameTimer::getTime(int unit)
{
    unsigned long long curTime;

    QueryPerformanceCounter(&mEnd);

    double curSec = (double)(mEnd.QuadPart - mStart.QuadPart)/(double)mFreq.QuadPart;
    switch (unit) 
    {
    case GAME_TIMEUNIT_USEC: curTime = curSec * 1000000.0; break;
    case GAME_TIMEUNIT_MSEC: curTime = curSec * 1000.0; break;
    case GAME_TIMEUNIT_SEC: curTime = curSec; break;
    }

    return (unsigned long)curTime;
}

bool WindowMasterGameTimer::pause(void)
{
    return false;
}

bool WindowMasterGameTimer::resume(void)
{
    return false;
}

