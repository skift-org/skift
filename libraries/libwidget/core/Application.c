#include <libsystem/assert.h>
#include <libsystem/logger.h>
#include <libsystem/messageloop.h>
#include <libwidget/core/Application.h>

static bool _initialized = false;
static bool _running = false;
static int _exit_value = 0;
static List *_windows;

void application_initialize(int argc, char **argv)
{
    assert(!_initialized);

    _windows = list_create();

    messageloop_init(argc, argv);

    _initialized = true;
}

int application_run(void)
{
    assert(_initialized);
    assert(!_running);

    _running = true;
    while (_running)
    {
        messageloop_pump(true);
    }

    messageloop_fini();

    return _exit_value;
}

void application_exit(int exit_value)
{
    assert(_initialized);
    assert(_running);

    _exit_value = exit_value;
    _running = false;
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
