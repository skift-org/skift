#include <libsystem/assert.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/logger.h>

#include <libwidget/core/Application.h>

static bool _initialized = false;
static bool _running = false;
static List *_windows;

void application_initialize(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    assert(!_initialized);

    _windows = list_create();

    eventloop_initilize();

    _initialized = true;
}

int application_run(void)
{
    assert(_initialized);
    assert(!_running);

    return eventloop_run();
}

void application_exit(int exit_value)
{
    assert(_initialized);
    assert(_running);

    eventloop_exit(exit_value);
}

void application_dump(void)
{
    assert(_initialized);

    list_foreach(Widget, window, _windows)
    {
        widget_dump(window);
    }
}

void application_add_window(Window *window)
{
    assert(_initialized);

    logger_info("Adding %s(0x%08x)", WIDGET(window)->classname, window);

    list_pushback(_windows, window);
}

void application_remove_window(Window *window)
{
    assert(_initialized);

    logger_info("Removing %s(0x%08x)", WIDGET(window)->classname, window);

    list_remove(_windows, window);
}
