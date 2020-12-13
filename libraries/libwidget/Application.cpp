#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>
#include <libsystem/process/Process.h>
#include <libsystem/utils/Hexdump.h>

#include <libwidget/Application.h>
#include <libwidget/Screen.h>

#include "compositor/Protocol.h"

namespace Application
{

enum class State
{
    UNINITIALIZED,
    INITALIZED,
    EXITING,
};

State _state = State::UNINITIALIZED;
List *_windows;
Connection *_connection;
Notifier *_connection_notifier;
bool _wireframe = false;

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

        list_foreach(Window, window, _windows)
        {
            Event event = {
                .type = Event::DISPLAY_SIZE_CHANGED,
                .accepted = false,
                .mouse = {},
                .keyboard = {},
            };

            window->dispatch_event(&event);
        }
    }
    else
    {
        logger_error("Got an invalid message from compositor (%d)!", message.type);
        hexdump(&message, sizeof(CompositorMessage));
        Application::exit(-1);
    }
}

void send_message(CompositorMessage message)
{
    connection_send(_connection, &message, sizeof(CompositorMessage));
}

ResultOr<CompositorMessage> wait_for_message(CompositorMessageType expected_message)
{
    Vector<CompositorMessage> pendings;

    CompositorMessage message{};
    connection_receive(_connection, &message, sizeof(CompositorMessage));

    if (handle_has_error(_connection))
    {
        return handle_get_error(_connection);
    }

    while (message.type != expected_message)
    {
        pendings.push_back(move(message));
        connection_receive(_connection, &message, sizeof(CompositorMessage));

        if (handle_has_error(_connection))
        {
            pendings.foreach ([&](auto &message) {
                do_message(message);
                return Iteration::CONTINUE;
            });

            return handle_get_error(_connection);
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

void request_callback(void *target, Connection *connection, PollEvent events)
{
    __unused(target);
    __unused(events);

    CompositorMessage message = {};
    memset((void *)&message, 0xff, sizeof(CompositorMessage));
    size_t message_size = connection_receive(connection, &message, sizeof(CompositorMessage));

    if (handle_has_error(connection))
    {
        logger_error("Connection to the compositor closed %s!", handle_error_string(connection));
        Application::exit(-1);
    }

    if (message_size != sizeof(CompositorMessage))
    {
        logger_error("Got a message with an invalid size from compositor %u != %u!", sizeof(CompositorMessage), message_size);
        hexdump(&message, message_size);
        Application::exit(-1);
    }

    do_message(message);
}

Result initialize(int argc, char **argv)
{
    assert(_state == State::UNINITIALIZED);

    bool theme_changed = false;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--theme") == 0 && i + 1 < argc)
        {
            char buffer[256];
            snprintf(buffer, 256, "/Files/Themes/%s.json", argv[i + 1]);
            theme_load(buffer);
            theme_changed = true;
        }

        if (strcmp(argv[i], "--wireframe") == 0)
        {
            _wireframe = true;
        }
    }

    if (!theme_changed)
    {
        theme_load("/Files/Themes/skift-dark.json");
    }

    _connection = socket_connect("/Session/compositor.ipc");

    if (handle_has_error(_connection))
    {
        logger_error("Failed to connect to the compositor: %s", handle_error_string(_connection));
        Result result = handle_get_error(_connection);
        connection_close(_connection);
        _connection = nullptr;

        return result;
    }

    _windows = list_create();

    eventloop_initialize();

    _connection_notifier = notifier_create(
        nullptr,
        HANDLE(_connection),
        POLL_READ,
        (NotifierCallback)request_callback);

    _state = State::INITALIZED;

    auto result_or_greetings = wait_for_message(COMPOSITOR_MESSAGE_GREETINGS);

    if (result_or_greetings.success())
    {
        auto greetings = result_or_greetings.take_value();

        Screen::bound(greetings.greetings.screen_bound);
    }

    return SUCCESS;
}

int run()
{
    assert(_state == State::INITALIZED);

    return eventloop_run();
}

int run_nested()
{
    assert(_state == State::INITALIZED);

    return eventloop_run_nested();
}

int pump()
{
    assert(_state == State::INITALIZED);

    eventloop_pump(true);

    return 0;
}

void exit(int exit_value)
{
    assert(_state == State::INITALIZED);
    _state = State::EXITING;

    Window *window = (Window *)list_peek(_windows);
    while (window)
    {
        delete window;
        window = (Window *)list_peek(_windows);
    }

    eventloop_exit(exit_value);

    _state = State::UNINITIALIZED;
}

void exit_nested(int exit_value)
{
    assert(_state == State::INITALIZED);
    eventloop_exit_nested(exit_value);
}

bool show_wireframe()
{
    return _wireframe;
}

void exit_if_all_windows_are_closed()
{
    if (_state == State::EXITING)
    {
        return;
    }

    bool should_close = true;

    list_foreach(Window, window, _windows)
    {
        if (window->visible())
        {
            should_close = false;
        }
    }

    if (should_close)
    {
        exit(0);
    }
}

void add_window(Window *window)
{
    assert(_state >= State::INITALIZED);

    list_pushback(_windows, window);
}

void remove_window(Window *window)
{
    assert(_state >= State::INITALIZED);

    list_remove(_windows, window);
}

Window *get_window(int id)
{
    assert(_state >= State::INITALIZED);

    list_foreach(Window, window, _windows)
    {
        if (window->handle() == id)
        {
            return window;
        }
    }

    return nullptr;
}

void show_window(Window *window)
{
    assert(_state >= State::INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_CREATE_WINDOW,
        .create_window = {
            .id = window->handle(),
            .flags = window->_flags,
            .type = window->type(),
            .frontbuffer = window->frontbuffer_handle(),
            .frontbuffer_size = window->frontbuffer->size(),
            .backbuffer = window->backbuffer_handle(),
            .backbuffer_size = window->frontbuffer->size(),
            .bound = window->bound_on_screen(),
        },
    };

    send_message(message);
}

void hide_window(Window *window)
{
    assert(_state >= State::INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_DESTROY_WINDOW,
        .destroy_window = {
            .id = window->handle(),
        },
    };

    send_message(message);
    exit_if_all_windows_are_closed();
}

void flip_window(Window *window, Recti bound)
{
    assert(_state >= State::INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_FLIP_WINDOW,
        .flip_window = {
            .id = window->handle(),
            .frontbuffer = window->frontbuffer_handle(),
            .frontbuffer_size = window->frontbuffer->size(),
            .backbuffer = window->backbuffer_handle(),
            .backbuffer_size = window->frontbuffer->size(),
            .bound = bound,
        },
    };

    send_message(message);
    wait_for_ack();
}

void move_window(Window *window, Vec2i position)
{
    assert(_state >= State::INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_MOVE_WINDOW,
        .move_window = {
            .id = window->handle(),
            .position = position,
        },
    };

    send_message(message);
}

void resize_window(Window *window, Recti bound)
{
    assert(_state >= State::INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_RESIZE_WINDOW,
        .resize_window = {
            .id = window->handle(),
            .bound = bound,
        },
    };

    send_message(message);
}

void window_change_cursor(Window *window, CursorState state)
{
    assert(_state >= State::INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_CURSOR_WINDOW,
        .cursor_window = {
            .id = window->handle(),
            .state = state,
        },
    };

    send_message(message);
}

Vec2i mouse_position()
{
    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_GET_MOUSE_POSITION,
        .mouse_position = {},
    };

    send_message(message);

    auto result_or_mouse_position = wait_for_message(COMPOSITOR_MESSAGE_MOUSE_POSITION);

    if (result_or_mouse_position.success())
    {
        auto mouse_position = result_or_mouse_position.value();

        return mouse_position.mouse_position.position;
    }
    else
    {
        return Vec2i::zero();
    }
}

} // namespace Application
