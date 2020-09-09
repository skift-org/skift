#include <abi/Paths.h>

#include <libsystem/Logger.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/process/Launchpad.h>
#include <libwidget/Application.h>

#include "terminal/FramebufferTerminal.h"
#include "terminal/TerminalWidget.h"
#include "terminal/TextmodeTerminal.h"

#define TERMINAL_IO_BUFFER_SIZE 4096

Stream *master = nullptr, *slave = nullptr;

void keyboard_callback(void *target, Stream *keyboard, SelectEvent events)
{
    __unused(target);
    __unused(events);

    char buffer[TERMINAL_IO_BUFFER_SIZE];
    size_t size = stream_read(keyboard, buffer, TERMINAL_IO_BUFFER_SIZE);

    stream_write(master, buffer, size);
}

void master_callback(Terminal *terminal, Stream *master, SelectEvent events)
{
    __unused(events);

    char buffer[TERMINAL_IO_BUFFER_SIZE];
    size_t size = stream_read(master, buffer, TERMINAL_IO_BUFFER_SIZE);

    if (handle_has_error(master))
    {
        handle_printf_error(master, "Terminal: read from master failed");
        return;
    }

    terminal_write(terminal, buffer, size);
}

void repaint_callback(Terminal *terminal)
{
    terminal_repaint(terminal);
}

int main(int argc, char **argv)
{
    if (application_initialize(argc, argv) == SUCCESS)
    {
        Window *window = window_create(WINDOW_RESIZABLE | WINDOW_TRANSPARENT);

        window->icon(Icon::get("console-line"));
        window->title("Terminal");
        window->size(Vec2i(500, 400));
        window->opacity(1);

        Widget *widget = new TerminalWidget(window->root());
        widget->focus();

        window->show();

        return application_run();
    }
    else
    {
        Result result = stream_create_term(&master, &slave);

        if (result != SUCCESS)
        {
            logger_error("Failed to create the terminal device: %s", result_to_string(result));
            return -1;
        }

        Stream *keyboard = stream_open(KEYBOARD_DEVICE_PATH, OPEN_READ);

        if (handle_has_error(keyboard))
        {
            logger_error("Failed to open the keyboard device: %s", handle_get_error(keyboard));
            return -1;
        }

        Terminal *terminal = framebuffer_terminal_create();

        if (!terminal)
        {
            terminal = textmode_terminal_create();

            if (!terminal)
            {
                logger_error("Failed to open a device to display the terminal in!");
                return -1;
            }
        }

        eventloop_initialize();

        notifier_create(terminal, HANDLE(keyboard), SELECT_READ, (NotifierCallback)keyboard_callback);
        notifier_create(terminal, HANDLE(master), SELECT_READ, (NotifierCallback)master_callback);

        auto blink_timer = make<Timer>(250, [&]() { terminal_blink(terminal); });
        blink_timer->start();

        auto repaint_timer = make<Timer>(1000 / 60, [&]() { terminal_repaint(terminal); });
        repaint_timer->start();

        Launchpad *shell_launchpad = launchpad_create("shell", "/Applications/shell/shell");
        launchpad_handle(shell_launchpad, HANDLE(slave), 0);
        launchpad_handle(shell_launchpad, HANDLE(slave), 1);
        launchpad_handle(shell_launchpad, HANDLE(slave), 2);
        launchpad_launch(shell_launchpad, nullptr);

        return eventloop_run();
    }
}
