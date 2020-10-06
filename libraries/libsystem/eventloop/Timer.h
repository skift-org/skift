#pragma once

#include <libsystem/Time.h>
#include <libutils/Callback.h>
#include <libutils/RefPtr.h>

class Timer
{
private:
    bool _running = false;
    TimeStamp _scheduled = 0;
    Timeout _interval = 0;
    Callback<void()> _callback;

public:
    auto running() { return _running; }

    auto interval() { return _interval; }

    auto scheduled() { return _scheduled; }

    void schedule(TimeStamp when) { _scheduled = when; }

    void trigger()
    {
        _callback();
    }

    Timer(Timeout interval, Callback<void()> callback);

    ~Timer();

    void start();

    void stop();
};
