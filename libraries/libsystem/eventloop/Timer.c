#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Timer.h>

Timer *timer_create(Timeout interval, TimerCallback callback)
{
    Timer *timer = __create(Timer);

    timer->started = false;
    timer->elapsed = 0;
    timer->interval = interval;
    timer->callback = callback;

    eventloop_register_timer(timer);

    return timer;
}

void timer_destroy(Timer *timer)
{
    eventloop_unregister_timer(timer);

    if (timer->started)
    {
        timer_stop(timer);
    }

    free(timer);
}

void timer_start(Timer *timer)
{
    timer->started = true;
}

void timer_stop(Timer *timer)
{
    timer->started = false;
}
