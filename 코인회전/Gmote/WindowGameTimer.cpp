#include "stdafx.h"


#include "WindowGameTimer.h"



WindowGameTimer::WindowGameTimer(void)
{
    mPaused = false;
    mStop = true;
    mStart.QuadPart = 0;
    mEnd.QuadPart = 0;
    mPause.QuadPart = 0;
    mResume.QuadPart = 0;
    mSupported = QueryPerformanceFrequency(&mFreq);
}

bool WindowGameTimer::start(void)
{
    mStop=false;
    QueryPerformanceCounter(&mStart);
    return mSupported;
}

bool WindowGameTimer::stop(void)
{
    if(!mStop)
    {
        mStop = true;
        mPaused = false;
        return true;
    }
    else
        return false;
}

unsigned long WindowGameTimer::getTime(int unit)
{
    unsigned long long curTime;
    if(mStop)
        return 0;

    QueryPerformanceCounter(&mEnd);

    double curSec = (!mPaused) ? (double)(mEnd.QuadPart - mStart.QuadPart)/(double)mFreq.QuadPart : (mPause.QuadPart - mStart.QuadPart)/(double)mFreq.QuadPart;  
    switch (unit) 
    {
    case GAME_TIMEUNIT_USEC: curTime = curSec * 1000000.0; break;
    case GAME_TIMEUNIT_MSEC: curTime = curSec * 1000.0; break;
    case GAME_TIMEUNIT_SEC: curTime = curSec; break;
    }

    return (unsigned long)curTime;
}

bool WindowGameTimer::pause(void)
{
    if(!mPaused)
    {
        mPaused = true;
        QueryPerformanceCounter(&mPause);
        return true;
    }
    else
        return false;
}

bool WindowGameTimer::resume(void)
{
    if(mPaused)
    {
        QueryPerformanceCounter(&mResume);
        mStart.QuadPart += (mResume.QuadPart - mPause.QuadPart);
        mPaused = false;
        return true;
    }
    else
        return false;
}

