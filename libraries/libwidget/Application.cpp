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

enum ApplicationState
{
    APPLICATION_NONE,

    APPLICATION_INITALIZED,
    APPLICATION_RUNNING,
    APPLICATION_EXITING,
};

static ApplicationState _state = APPLICATION_NONE;
static List *_windows;
static Connection *_connection;
static Notifier *_connection_notifier;
static bool _is_debbuging_layout = false;

void application_do_message(CompositorMessage *message)
{
    if (message->type == COMPOSITOR_MESSAGE_EVENT_WINDOW)
    {
        Window *window = application_get_window(message->event_window.id);

        if (window)
        {
            window->dispatch_event(&message->event_window.event);
        }
    }
    else if (message->type == COMPOSITOR_MESSAGE_CHANGED_RESOLUTION)
    {
        Screen::bound(message->changed_resolution.resolution);

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
        logger_error("Got an invalid message from compositor (%d)!", message->type);
        hexdump(&message, sizeof(CompositorMessage));
        application_exit(-1);
    }
}

void application_send_message(CompositorMessage message)
{
    connection_send(_connection, &message, sizeof(CompositorMessage));
}

CompositorMessage *application_wait_for_message(CompositorMessageType expected_message)
{
    List *pending_messages = nullptr;

    CompositorMessage *message = __create(CompositorMessage);
    connection_receive(_connection, message, sizeof(CompositorMessage));

    while (message->type != expected_message)
    {
        if (pending_messages == nullptr)
        {
            pending_messages = list_create();
        }

        list_pushback(pending_messages, message);
        message = __create(CompositorMessage);
        connection_receive(_connection, message, sizeof(CompositorMessage));
    }

    if (pending_messages)
    {
        list_foreach(CompositorMessage, message, pending_messages)
        {
            application_do_message(message);
        }

        list_destroy_with_callback(pending_messages, free);
    }

    return message;
}

void application_wait_for_ack()
{
    CompositorMessage *ack_message = application_wait_for_message(COMPOSITOR_MESSAGE_ACK);
    if (ack_message)
    {
        free(ack_message);
    }
}

void application_request_callback(
    void *target,
    Connection *connection,
    PollEvent events)
{
    __unused(target);
    __unused(events);

    CompositorMessage message = {};
    memset((void *)&message, 0xff, sizeof(CompositorMessage));
    size_t message_size = connection_receive(connection, &message, sizeof(CompositorMessage));

    if (handle_has_error(connection))
    {
        logger_error("Connection to the compositor closed %s!", handle_error_string(connection));
        application_exit(-1);
    }

    if (message_size != sizeof(CompositorMessage))
    {
        logger_error("Got a message with an invalid size from compositor %u != %u!", sizeof(CompositorMessage), message_size);
        hexdump(&message, message_size);
        application_exit(-1);
    }

    application_do_message(&message);
}

Result application_initialize(int argc, char **argv)
{
    assert(_state == APPLICATION_NONE);

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

        if (strcmp(argv[i], "--debug-layout") == 0)
        {
            _is_debbuging_layout = true;
        }
    }

    if (!theme_changed)
        theme_load("/Files/Themes/skift-dark.json");

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
        (NotifierCallback)application_request_callback);

    _state = APPLICATION_INITALIZED;

    CompositorMessage *greetings_message = application_wait_for_message(COMPOSITOR_MESSAGE_GREETINGS);

    if (greetings_message)
    {
        Screen::bound((greetings_message->greetings.screen_bound));
    }

    return SUCCESS;
}

int application_run()
{
    assert(_state == APPLICATION_INITALIZED);
    _state = APPLICATION_RUNNING;

    return eventloop_run();
}

int application_run_nested()
{
    assert(_state == APPLICATION_RUNNING);

    return eventloop_run_nested();
}

int application_pump()
{
    if (_state == APPLICATION_INITALIZED)
    {
        _state = APPLICATION_RUNNING;
    }

    eventloop_pump(true);

    return 0;
}

void application_exit(int exit_value)
{
    assert(_state == APPLICATION_RUNNING);
    _state = APPLICATION_EXITING;

    Window *window = (Window *)list_peek(_windows);
    while (window)
    {
        delete window;
        window = (Window *)list_peek(_windows);
    }

    eventloop_exit(exit_value);

    _state = APPLICATION_NONE;
}

void application_exit_nested(int exit_value)
{
    assert(_state == APPLICATION_RUNNING);
    eventloop_exit_nested(exit_value);
}

bool application_is_debbuging_layout()
{
    return _is_debbuging_layout;
}

void application_exit_if_all_windows_are_closed()
{
    if (_state == APPLICATION_EXITING)
    {
        return;
    }

    bool should_application_close = true;

    list_foreach(Window, window, _windows)
    {
        if (window->visible())
        {
            should_application_close = false;
        }
    }

    if (should_application_close)
    {
        application_exit(0);
    }
}

void application_add_window(Window *window)
{
    assert(_state >= APPLICATION_INITALIZED);

    list_pushback(_windows, window);
}

void application_remove_window(Window *window)
{
    assert(_state >= APPLICATION_INITALIZED);

    list_remove(_windows, window);
}

Window *application_get_window(int id)
{
    assert(_state >= APPLICATION_INITALIZED);

    list_foreach(Window, window, _windows)
    {
        if (window->handle() == id)
        {
            return window;
        }
    }

    return nullptr;
}

void application_show_window(Window *window)
{
    assert(_state >= APPLICATION_INITALIZED);
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

    application_send_message(message);
}

void application_hide_window(Window *window)
{
    assert(_state >= APPLICATION_INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_DESTROY_WINDOW,
        .destroy_window = {
            .id = window->handle(),
        },
    };

    application_send_message(message);
    application_exit_if_all_windows_are_closed();
}

void application_flip_window(Window *window, Rectangle bound)
{
    assert(_state >= APPLICATION_INITALIZED);
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

    application_send_message(message);
    application_wait_for_ack();
}

void application_move_window(Window *window, Vec2i position)
{
    assert(_state >= APPLICATION_INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_MOVE_WINDOW,
        .move_window = {
            .id = window->handle(),
            .position = position,
        },
    };

    application_send_message(message);
}

void application_resize_window(Window *window, Rectangle bound)
{
    assert(_state >= APPLICATION_INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_RESIZE_WINDOW,
        .resize_window = {
            .id = window->handle(),
            .bound = bound,
        },
    };

    application_send_message(message);
}

void application_window_change_cursor(Window *window, CursorState state)
{
    assert(_state >= APPLICATION_INITALIZED);
    assert(list_contains(_windows, window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_CURSOR_WINDOW,
        .cursor_window = {
            .id = window->handle(),
            .state = state,
        },
    };

    application_send_message(message);
}

Vec2i application_get_mouse_position()
{
    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_GET_MOUSE_POSITION,
        .mouse_position = {},
    };

    application_send_message(message);

    auto resp = application_wait_for_message(COMPOSITOR_MESSAGE_MOUSE_POSITION);

    auto result = Vec2i::zero();

    if (resp)
    {
        result = resp->mouse_position.position;
        free(resp);
    }

    return result;
}
