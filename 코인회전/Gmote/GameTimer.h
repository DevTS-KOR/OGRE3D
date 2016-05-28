#if !defined(__GAME_TIMER_H__)
#define __GAME_TIMER_H__

#define GAME_TIMEUNIT_MSEC  1
#define GAME_TIMEUNIT_SEC    2
#define GAME_TIMEUNIT_USEC  3

class GameTimer
{
public:

    virtual bool start(void) = 0;
    virtual bool stop(void) = 0;

    virtual bool pause(void) = 0;
    virtual bool resume(void) = 0;

    virtual unsigned long getTime(int unit) = 0;
    virtual unsigned long getTime(void) = 0;
};

#endif

