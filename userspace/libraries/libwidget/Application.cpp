#include <assert.h>

#include <libasync/Loop.h>
#include <libasync/Notifier.h>
#include <libio/Connection.h>
#include <libio/Format.h>
#include <libio/Socket.h>
#include <libsettings/Setting.h>
#include <libsystem/Logger.h>
#include <libsystem/process/Process.h>
#include <libsystem/utils/Hexdump.h>
#include <libwidget/Application.h>
#include <libwidget/Screen.h>

#include "compositor/Protocol.h"

namespace Widget
{

namespace Application
{

/* --- Context -------------------------------------------------------------- */

enum class State
{
    UNINITIALIZED,
    RUNNING,
    EXITING,
};

Vector<Window *> _windows;
State _state = State::UNINITIALIZED;
IO::Connection _connection;
OwnPtr<Async::Notifier> _connection_notifier;
bool _wireframe = false;

/* --- IPC ------------------------------------------------------------------ */

void send_message(CompositorMessage message)
{
    _connection.write(&message, sizeof(message));
}

void do_message(const CompositorMessage &message)
{
    if (message.type == COMPOSITOR_MESSAGE_EVENT_WINDOW)
    {
        Window *window = get_window(message.event_window.id);

        if (window)
        {
            Event copy = message.event_window.event;
            window->dispatch_event(&copy);
        }
    }
    else if (message.type == COMPOSITOR_MESSAGE_CHANGED_RESOLUTION)
    {
        Screen::bound(message.changed_resolution.resolution);

        for (size_t i = 0; i < _windows.count(); i++)
        {
            Event e = {
                .type = Event::DISPLAY_SIZE_CHANGED,
                .accepted = false,
                .mouse = {},
                .keyboard = {},
            };

            _windows[i]->dispatch_event(&e);
        }
    }
    else
    {
        logger_error("Got an invalid message from compositor (%d)!", message.type);
        hexdump(&message, sizeof(CompositorMessage));
        Application::exit(PROCESS_FAILURE);
    }
}

ResultOr<CompositorMessage> wait_for_message(CompositorMessageType expected_message)
{
    Vector<CompositorMessage> pendings;

    CompositorMessage message{};
    TRY(_connection.read(&message, sizeof(message)));

    while (message.type != expected_message)
    {
        pendings.push_back(move(message));
        auto result = _connection.read(&message, sizeof(CompositorMessage));

        if (!result.success())
        {
            pendings.foreach ([&](auto &message) {
                do_message(message);
                return Iteration::CONTINUE;
            });

            return result.result();
        }
    }

    pendings.foreach ([&](auto &message) {
        do_message(message);
        return Iteration::CONTINUE;
    });

    return message;
}

void wait_for_ack()
{
    wait_for_message(COMPOSITOR_MESSAGE_ACK);
}

bool show_wireframe()
{
    return _wireframe;
}

void show_window(Window *window)
{
    assert(_state >= State::RUNNING);
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_CREATE_WINDOW,
        .create_window = {
            .id = window->handle(),
            .flags = window->flags(),
            .type = window->type(),
            .frontbuffer = window->frontbuffer_handle(),
            .frontbuffer_size = window->frontbuffer_size(),
            .backbuffer = window->backbuffer_handle(),
            .backbuffer_size = window->backbuffer_size(),
            .bound = window->bound_on_screen(),
        },
    };

    send_message(message);
}

void exit_if_all_windows_are_closed();

void hide_window(Window *window)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_DESTROY_WINDOW,
        .destroy_window = {
            .id = window->handle(),
        },
    };

    send_message(message);
    exit_if_all_windows_are_closed();
}

void flip_window(Window *window, Math::Recti dirty)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_FLIP_WINDOW,
        .flip_window = {
            .id = window->handle(),
            .frontbuffer = window->frontbuffer_handle(),
            .frontbuffer_size = window->frontbuffer_size(),
            .backbuffer = window->backbuffer_handle(),
            .backbuffer_size = window->backbuffer_size(),
            .dirty = dirty,
            .bound = window->bound_on_screen(),
        },
    };

    send_message(message);
    wait_for_ack();
}

void move_window(Window *window, Math::Vec2i position)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_MOVE_WINDOW,
        .move_window = {
            .id = window->handle(),
            .position = position,
        },
    };

    send_message(message);
}

void window_change_cursor(Window *window, CursorState state)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_CURSOR_WINDOW,
        .cursor_window = {
            .id = window->handle(),
            .state = state,
        },
    };

    send_message(message);
}

Math::Vec2i mouse_position()
{
    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_GET_MOUSE_POSITION,
        .mouse_position = {},
    };

    send_message(message);

    auto result_or_mouse_position = wait_for_message(COMPOSITOR_MESSAGE_MOUSE_POSITION);

    if (result_or_mouse_position.success())
    {
        auto mouse_position = result_or_mouse_position.unwrap();

        return mouse_position.mouse_position.position;
    }
    else
    {
        return Math::Vec2i::zero();
    }
}

void goodbye()
{
    CompositorMessage m = {
        .type = COMPOSITOR_MESSAGE_GOODBYE,
        .mouse_position = {},
    };

    send_message(m);

    wait_for_ack();
}

/* --- Windows -------------------------------------------------------------- */

void hide_all_windows()
{
    for (size_t i = 0; i < _windows.count(); i++)
    {
        _windows[i]->hide();
    }
}

void exit_if_all_windows_are_closed()
{
    if (_state == State::EXITING)
    {
        return;
    }

    bool should_close = true;

    for (size_t i = 0; i < _windows.count(); i++)
    {
        if (_windows[i]->visible())
        {
            should_close = false;
            break;
        }
    }

    if (should_close)
    {
        exit(PROCESS_SUCCESS);
    }
}

void add_window(Window *window)
{
    assert(_state >= State::RUNNING);

    _windows.push_back(window);
}

void remove_window(Window *window)
{
    _windows.push_back(window);
    exit_if_all_windows_are_closed();
}

Window *get_window(int id)
{
    assert(_state >= State::RUNNING);

    for (size_t i = 0; i < _windows.count(); i++)
    {
        if (_windows[i]->handle() == id)
        {
            return _windows[i];
        }
    }

    return nullptr;
}

/* --- Application ---------------------------------------------------------- */

void hide_all_windows();
void uninitialized();

static OwnPtr<Settings::Setting> _setting_theme;
static OwnPtr<Settings::Setting> _setting_wireframe;

Result initialize(int argc, char **argv)
{
    assert(_state == State::UNINITIALIZED);

    _setting_theme = own<Settings::Setting>(
        "appearance:widgets.theme",
        [](const Json::Value &value) {
            auto new_theme = value.as_string();

            theme_load(IO::format("/Files/Themes/{}.json", value.as_string()));

            for (size_t i = 0; i < _windows.count(); i++)
            {
                _windows[i]->should_repaint(_windows[i]->bound());
            }
        });

    _setting_wireframe = own<Settings::Setting>(
        "appearance:widgets.wireframe",
        [](const Json::Value &value) {
            _wireframe = value.as_bool();
            for (size_t i = 0; i < _windows.count(); i++)
            {
                _windows[i]->should_repaint(_windows[i]->bound());
            }
        });

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--wireframe") == 0)
        {
            _wireframe = true;
        }
    }

    _connection = TRY(IO::Socket::connect("/Session/compositor.ipc"));

    Async::Loop::initialize();

    _connection_notifier = own<Async::Notifier>(_connection, POLL_READ, []() {
        CompositorMessage message = {};
        auto read_result = _connection.read(&message, sizeof(CompositorMessage));

        if (!read_result.success())
        {
            logger_error("Connection to the compositor closed %s!", read_result.description());
            Application::exit(PROCESS_FAILURE);
        }

        size_t message_size = read_result.unwrap();

        if (message_size != sizeof(CompositorMessage))
        {
            logger_error("Got a message with an invalid size from compositor %u != %u!", sizeof(CompositorMessage), message_size);
            hexdump(&message, message_size);
            Application::exit(PROCESS_FAILURE);
        }

        do_message(message);
    });

    _state = State::RUNNING;

    Async::Loop::atexit(uninitialized);

    auto result_or_greetings = wait_for_message(COMPOSITOR_MESSAGE_GREETINGS);

    if (result_or_greetings.success())
    {
        auto greetings = result_or_greetings.unwrap();

        Screen::bound(greetings.greetings.screen_bound);
    }

    return SUCCESS;
}

void uninitialized()
{
    _state = State::UNINITIALIZED;
}

int run()
{
    assert(_state == State::RUNNING);

    return Async::Loop::run();
}

int run_nested()
{
    assert(_state == State::RUNNING);

    return Async::Loop::run_nested();
}

int pump()
{
    assert(_state == State::RUNNING);

    Async::Loop::pump(true);

    return 0;
}

void exit(int exit_value)
{
    if (_state == State::EXITING)
    {
        return;
    }

    assert(_state == State::RUNNING);
    _state = State::EXITING;

    hide_all_windows();

    goodbye();

    _connection_notifier = nullptr;
    _connection.close();

    Async::Loop::exit(exit_value);
}

void exit_nested(int exit_value)
{
    assert(_state == State::RUNNING);
    Async::Loop::exit_nested(exit_value);
}

} // namespace Application

} // namespace Widget
