#include <libasync/Loop.h>
#include <libasync/Timer.h>

namespace Async
{

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
        Loop::register_timer(this);
    }
}

void Timer::stop()
{
    if (_running)
    {
        _running = false;
        Loop::unregister_timer(this);
    }
}

} // namespace Async
