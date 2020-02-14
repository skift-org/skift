#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/logger.h>
#include <libsystem/process/Launchpad.h>

#include "Terminal/FramebufferTerminal.h"
#include "Terminal/TextmodeTerminal.h"

#define TERMINAL_IO_BUFFER_SIZE 4096

Terminal *terminal = NULL;
Stream *master = NULL, *slave = NULL;

void keyboard_callback(Notifier *notifier, Stream *keyboard)
{
    __unused(notifier);

    char buffer[TERMINAL_IO_BUFFER_SIZE];
    size_t size = stream_read(keyboard, buffer, TERMINAL_IO_BUFFER_SIZE);

    stream_write(master, buffer, size);
}

void master_callback(Notifier *notifier, Stream *master)
{
    __unused(notifier);

    char buffer[TERMINAL_IO_BUFFER_SIZE];
    size_t size = stream_read(master, buffer, TERMINAL_IO_BUFFER_SIZE);

    if (handle_has_error(master))
    {
        handle_printf_error(master, "Terminal: read from master failled");
        return;
    }

    terminal_write(terminal, buffer, size);
}

void repaint_callback(Timer *timer)
{
    __unused(timer);

    terminal_repaint(terminal);
}

void cursor_callback(Timer *timer)
{
    __unused(timer);

    terminal_blink(terminal);
}

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);

    eventloop_initialize();

    Stream *keyboard = stream_open("/dev/keyboard", OPEN_READ);

    if (handle_has_error(keyboard))
    {
        logger_error("Failled to open the keyboard device: %s", handle_get_error(keyboard));
        return -1;
    }

    Result result = stream_create_term(&master, &slave);

    if (result != SUCCESS)
    {
        logger_error("Failled to create the terminal device: %s", result_to_string(result));
        return -1;
    }

    terminal = framebuffer_terminal_create();

    if (!terminal)
    {
        terminal = textmode_terminal_create();

        if (!terminal)
        {
            logger_error("Failled to open a device to display the terminal in!");
            return -1;
        }
    }

    Notifier *keyboard_notifier = notifier_create(HANDLE(keyboard), SELECT_READ);

    keyboard_notifier->on_ready_to_read = (NotifierHandler)keyboard_callback;

    Notifier *master_notifier = notifier_create(HANDLE(master), SELECT_READ);

    master_notifier->on_ready_to_read = (NotifierHandler)master_callback;

    Timer *cursor_blink = timer_create(250, cursor_callback);
    timer_start(cursor_blink);

    Timer *repaint_timer = timer_create(16, repaint_callback);
    timer_start(repaint_timer);

    logger_trace("Starting the shell application...");

    Launchpad *shell_launchpad = launchpad_create("sh", "/bin/Shell");
    launchpad_handle(shell_launchpad, HANDLE(slave), 0);
    launchpad_handle(shell_launchpad, HANDLE(slave), 1);
    launchpad_handle(shell_launchpad, HANDLE(slave), 2);
    launchpad_launch(shell_launchpad);

    logger_trace("Shell application started");

    return eventloop_run();
}
