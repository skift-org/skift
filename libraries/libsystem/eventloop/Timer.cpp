#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Timer.h>

Timer::Timer(Timeout interval, Callback<void()> callback)
    : _interval(interval),
      _callback(callback)
{
}

Timer::~Timer()
{
    stop();
}

void Timer::start()
{
    if (!_running)
    {
        _running = true;
        eventloop_register_timer(this);
    }
}

void Timer::stop()
{
    if (_running)
    {
        _running = false;
        eventloop_unregister_timer(this);
    }
}
