#pragma once

#include <skift/Time.h>

#include <libasync/Source.h>
#include <libutils/Func.h>
#include <libutils/RefPtr.h>

namespace Async
{

struct Timer :
    public Source
{
private:
    bool _running = false;
    TimeStamp _scheduled = 0;
    Timeout _interval = 0;
    Func<void()> _callback;

public:
    auto running() { return _running; }

    auto interval() { return _interval; }

    void interval(Timeout interval) { _interval = interval; }

    auto scheduled() { return _scheduled; }

    void schedule(TimeStamp when) { _scheduled = when; }

    void trigger()
    {
        _callback();
    }

    Timer(Timeout interval, Func<void()> callback)
        : _interval(interval),
          _callback(callback)
    {
    }

    ~Timer()
    {
        stop();
    }

    void start()
    {
        if (!_running)
        {
            _running = true;
            loop().register_timer(this);
        }
    }

    void stop()
    {
        if (_running)
        {
            _running = false;
            loop().unregister_timer(this);
        }
    }
};

} // namespace Async
