#if !defined(__WINDOW_GAME_TIMER_H__)
#define __WINDOW_GAME_TIMER_H__

#define masterGameTimer (WindowMasterGameTimer::instance())

#include <windows.h>

#include "Singleton.h"
#include "GameTimer.h"


class WindowMasterGameTimer : public GameTimer, public SisaSingleton<WindowMasterGameTimer>
{
public:
    WindowMasterGameTimer(void);
    ~WindowMasterGameTimer(void) {}

    bool start(void);
    bool stop(void);

    bool pause(void);
    bool resume(void);

    unsigned long getTime(int unit);
    unsigned long getTime(void) { return getTime(GAME_TIMEUNIT_MSEC); }

private:
    LARGE_INTEGER mFreq, mStart, mEnd;
    bool mSupported;

};

class WindowGameTimer : public GameTimer
{
public:
    WindowGameTimer(void);
    ~WindowGameTimer(void) {}

    bool start(void);
    bool stop(void);

    bool pause(void);
    bool resume(void);

    unsigned long getTime(int unit);
    unsigned long getTime(void) { return getTime(GAME_TIMEUNIT_MSEC); }

private:
    LARGE_INTEGER mFreq, mStart, mEnd;
    LARGE_INTEGER mPause,mResume;
    bool mSupported;
    bool mPaused;
    bool mStop;

};


#endif