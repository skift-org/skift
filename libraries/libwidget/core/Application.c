#include <libsystem/assert.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>
#include <libsystem/logger.h>
#include <libsystem/process/Process.h>

#include <libwidget/core/Application.h>

#include "Compositor/Protocol.h"

static bool _initialized = false;
static bool _running = false;
static List *_windows;
static Connection *_connection;
static Notifier *_connection_notifier;

void application_request_callback(Notifier *notifier, Connection *connection)
{
    __unused(notifier);

    CompositorMessage header = {};
    connection_receive(connection, &header, sizeof(CompositorMessage));

    if (header.type == COMPOSITOR_MESSAGE_WINDOW_EVENT)
    {
        CompositorWindowEvent windowEvent = {};

        connection_receive(connection, &windowEvent, sizeof(CompositorWindowEvent));

        Event *event = malloc(header.size);
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

void application_initialize(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    logger_trace("Connecting to compositor...");
    _connection = socket_connect("/srv/compositor.ipc");

    if (handle_has_error(_connection))
    {
        logger_error("Failled to connect to the compositor: %s", handle_error_string(_connection));
        process_exit(-1);
    }
    else
    {
        logger_trace("Connected to compositor!");
    }

    assert(!_initialized);

    _windows = list_create();

    eventloop_initialize();

    _connection_notifier = notifier_create(HANDLE(_connection), SELECT_READ);
    _connection_notifier->on_ready_to_read = (NotifierHandler)application_request_callback;

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
    assert(_initialized);

    logger_info("Adding Window(0x%08x)", window);

    CompositorCreateWindowMessage message = {
        .id = window->id,
        .framebuffer = window->framebuffer_handle,
        .bound = window->bound,
    };

    application_send_message(COMPOSITOR_MESSAGE_CREATE_WINDOW, &message, sizeof(CompositorCreateWindowMessage));

    list_pushback(_windows, window);
}

void application_remove_window(Window *window)
{
    assert(_initialized);

    logger_info("Removing Window(0x%08x)", window);

    CompositorDestroyWindowMessage message = {
        .id = window->id,
    };

    application_send_message(COMPOSITOR_MESSAGE_DESTROY_WINDOW, &message, sizeof(CompositorDestroyWindowMessage));

    list_remove(_windows, window);
}

Window *application_get_window_by_id(int id)
{
    list_foreach(Window, window, _windows)
    {
        if (window->id == id)
        {
            return window;
        }
    }

    return NULL;
}

void application_blit_window(Window *window, Rectangle bound)
{
    assert(_initialized);

    CompositorBlitWindowMessage message = {
        .id = window->id,
        .bound = bound,
    };

    application_send_message(COMPOSITOR_MESSAGE_BLIT_WINDOW, &message, sizeof(CompositorBlitWindowMessage));
}

void application_move_window(Window *window, Point position)
{
    assert(_initialized);

    CompositorWindowMove message = {
        .id = window->id,
        .position = position,
    };

    application_send_message(COMPOSITOR_MESSAGE_WINDOW_MOVE, &message, sizeof(CompositorWindowMove));
}