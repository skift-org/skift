#pragma once

#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>

typedef void (*DrawDemoCallback)(Painter *painter, Rectangle screen, double time);

static double _time = 0;
static DrawDemoCallback _callback = NULL;

void demo_on_timer_tick(Window *window)
{
    _callback(window->painter, window_content_bound(window), _time);
    application_blit_window(window, window_content_bound(window));

    _time += 1.0 / 60;
}

static int demo_start(int argc, char **argv, const char *name, DrawDemoCallback callback)
{
    Result result = application_initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    _callback = callback;
    Window *main_window = window_create(name, 500, 400);
    Timer *timer = timer_create(main_window, 16, (TimerCallback)demo_on_timer_tick);
    timer_start(timer);

    application_run();

    return 0;
}