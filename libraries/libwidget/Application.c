#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>
#include <libsystem/logger.h>
#include <libsystem/process/Process.h>

#include <libwidget/Application.h>

#include "compositor/Protocol.h"

typedef enum
{
    APPLICATION_NONE,

    APPLICATION_INITALIZED,
    APPLICATION_RUNNING,
    APPLICATION_EXITING,
} ApplicationState;

static ApplicationState _state = APPLICATION_NONE;
static List *_windows;
static Connection *_connection;
static Notifier *_connection_notifier;

void application_request_callback(
    void *target,
    Connection *connection,
    SelectEvent events)
{
    __unused(target);
    __unused(events);

    CompositorMessage header = {};
    connection_receive(connection, &header, sizeof(CompositorMessage));

    if (handle_get_error(connection) == ERR_STREAM_CLOSED)
    {
        logger_error("Connection to the compositor closed!");
        application_exit(-1);
        return;
    }

    if (header.type == COMPOSITOR_MESSAGE_WINDOW_EVENT)
    {
        CompositorWindowEvent windowEvent = {};

        connection_receive(connection, &windowEvent, sizeof(CompositorWindowEvent));

        Event *event = (Event *)malloc(header.size);
        connection_receive(connection, event, header.size - sizeof(CompositorWindowEvent));

        Window *window = application_get_window_by_id(windowEvent.id);

        if (window)
        {
            window_handle_event(window, event);
        }

        free(event);
    }
    else
    {
        logger_warn("Got an invalid message from compositor!");
    }
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
            snprintf(buffer, 256, "/res/theme/%s.json", argv[i + 1]);
            theme_load(buffer);
            theme_changed = true;
        }
    }

    if (!theme_changed)
        theme_load("/res/theme/skift-dark.json");

    _connection = socket_connect("/srv/compositor.ipc");

    if (handle_has_error(_connection))
    {
        logger_error("Failled to connect to the compositor: %s", handle_error_string(_connection));
        Result result = handle_get_error(_connection);
        connection_close(_connection);
        _connection = NULL;
        return result;
    }

    _windows = list_create();

    eventloop_initialize();

    _connection_notifier = notifier_create(
        NULL,
        HANDLE(_connection),
        SELECT_READ,
        (NotifierCallback)application_request_callback);

    _state = APPLICATION_INITALIZED;

    return SUCCESS;
}

int application_run(void)
{
    assert(_state == APPLICATION_INITALIZED);
    _state = APPLICATION_RUNNING;

    return eventloop_run();
}

void application_exit(int exit_value)
{
    logger_debug("Exiting application...");

    assert(_state == APPLICATION_RUNNING);
    _state = APPLICATION_EXITING;
    Window *window = NULL;

    while (list_peek(_windows, (void **)&window))
    {
        window_destroy(window);
    }

    eventloop_exit(exit_value);

    _state = APPLICATION_NONE;
}

void application_dump(void)
{
    assert(_state >= APPLICATION_INITALIZED);

    list_foreach(Window, window, _windows)
    {
        window_dump(window);
    }
}

void application_send_message(CompositorMessageType type, const void *buffer, size_t size)
{
    CompositorMessage header = {};
    header.type = type;
    header.size = size;

    connection_send(_connection, &header, sizeof(CompositorMessage));
    connection_send(_connection, buffer, size);
}

void application_add_window(Window *window)
{
    assert(_state >= APPLICATION_INITALIZED);

    list_pushback(_windows, window);
}

void application_show_window(Window *window)
{
    assert(_state >= APPLICATION_INITALIZED);
    assert(list_contains(_windows, window));

    CompositorCreateWindowMessage message = {
        .id = window_handle(window),
        .framebuffer = window_framebuffer_handle(window),
        .bound = window_bound_on_screen(window),
    };

    application_send_message(COMPOSITOR_MESSAGE_CREATE_WINDOW, &message, sizeof(CompositorCreateWindowMessage));
}

void application_hide_window(Window *window)
{
    assert(_state >= APPLICATION_INITALIZED);
    assert(list_contains(_windows, window));

    CompositorDestroyWindowMessage message = {
        .id = window_handle(window),
    };

    application_send_message(COMPOSITOR_MESSAGE_DESTROY_WINDOW, &message, sizeof(CompositorDestroyWindowMessage));

    if (_state == APPLICATION_EXITING)
    {
        return;
    }

    bool should_application_close = false;

    list_foreach(Window, window, _windows)
    {
        should_application_close |= !window_is_visible(window);
    }

    if (should_application_close)
    {
        application_exit(0);
    }
}

void application_remove_window(Window *window)
{
    assert(_state >= APPLICATION_INITALIZED);

    list_remove(_windows, window);
}

Window *application_get_window_by_id(int id)
{
    assert(_state >= APPLICATION_INITALIZED);

    list_foreach(Window, window, _windows)
    {
        if (window_handle(window) == id)
        {
            return window;
        }
    }

    return NULL;
}

void application_blit_window(Window *window, Rectangle bound)
{
    assert(_state >= APPLICATION_INITALIZED);

    CompositorBlitWindowMessage message = {
        .id = window_handle(window),
        .framebuffer = window_framebuffer_handle(window),
        .bound = bound,
    };

    application_send_message(COMPOSITOR_MESSAGE_BLIT_WINDOW, &message, sizeof(CompositorBlitWindowMessage));
}

void application_move_window(Window *window, Point position)
{
    assert(_state >= APPLICATION_INITALIZED);

    CompositorWindowMove message = {
        .id = window_handle(window),
        .position = position,
    };

    application_send_message(COMPOSITOR_MESSAGE_WINDOW_MOVE, &message, sizeof(CompositorWindowMove));
}

void application_resize_window(Window *window, Rectangle bound)
{
    assert(_state >= APPLICATION_INITALIZED);

    CompositorWindowResize message = {
        .id = window_handle(window),
        .bound = bound,
    };

    application_send_message(COMPOSITOR_MESSAGE_WINDOW_RESIZE, &message, sizeof(CompositorWindowResize));
}

void application_window_change_cursor(Window *window, CursorState state)
{
    assert(_state >= APPLICATION_INITALIZED);

    CompositorCursorStateChange message = {
        .id = window_handle(window),
        .state = state,
    };

    application_send_message(COMPOSITOR_MESSAGE_CURSOR_STATE_CHANGE, &message, sizeof(CompositorCursorStateChange));
}
