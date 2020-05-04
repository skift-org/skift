#include <libsystem/assert.h>
#include <libsystem/logger.h>
#include <libsystem/memory.h>

#include "compositor/Client.h"
#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Protocol.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

void client_request_callback(Client *client, Connection *connection, SelectEvent events)
{
    __unused(events);

    CompositorMessage header = {};

    connection_receive(connection, &header, sizeof(CompositorMessage));

    if (handle_has_error(connection))
    {
        client->disconnected = true;
        client_close_disconnected_clients();
        return;
    }

    switch (header.type)
    {
    case COMPOSITOR_MESSAGE_CREATE_WINDOW:
    {
        CompositorCreateWindowMessage create_window = {};
        connection_receive(connection, &create_window, sizeof(CompositorCreateWindowMessage));

        if (handle_has_error(connection))
        {
            client->disconnected = true;
            client_close_disconnected_clients();
            return;
        }

        size_t size;
        Bitmap *frontbuffer = NULL;
        int frontbuffer_handle = create_window.frontbuffer;
        shared_memory_include(frontbuffer_handle, (uintptr_t *)&frontbuffer, &size);

        Bitmap *backbuffer = NULL;
        int backbuffer_handle = create_window.backbuffer;
        shared_memory_include(backbuffer_handle, (uintptr_t *)&backbuffer, &size);

        window_create(
            create_window.id,
            client,
            create_window.bound,

            frontbuffer,
            frontbuffer_handle,

            backbuffer,
            backbuffer_handle);

        break;
    }
    case COMPOSITOR_MESSAGE_DESTROY_WINDOW:
    {
        CompositorDestroyWindowMessage destroy_window = {};
        connection_receive(connection, &destroy_window, sizeof(CompositorDestroyWindowMessage));

        if (handle_has_error(connection))
        {
            client->disconnected = true;
            client_close_disconnected_clients();
            return;
        }

        Window *window = manager_get_window(client, destroy_window.id);

        if (window)
        {
            shared_memory_free((uintptr_t)window->frontbuffer);
            window->frontbuffer = NULL;
            shared_memory_free((uintptr_t)window->backbuffer);
            window->backbuffer = NULL;

            window_destroy(window);
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", destroy_window.id, client);
        }

        break;
    }
    case COMPOSITOR_MESSAGE_FLIP_WINDOW:
    {
        CompositorFlipWindowMessage flip_window = {};
        connection_receive(connection, &flip_window, sizeof(CompositorFlipWindowMessage));

        if (handle_has_error(connection))
        {
            client->disconnected = true;
            client_close_disconnected_clients();

            return;
        }

        Window *window = manager_get_window(client, flip_window.id);

        if (window)
        {
            __swap(Bitmap *, window->frontbuffer, window->backbuffer);
            __swap(int, window->frontbuffer_handle, window->backbuffer_handle);

            if (window->frontbuffer_handle != flip_window.frontbuffer)
            {
                Bitmap *new_frontbuffer = NULL;

                size_t size;
                if (shared_memory_include(
                        flip_window.frontbuffer,
                        (uintptr_t *)&new_frontbuffer,
                        &size) == SUCCESS)
                {
                    shared_memory_free((uintptr_t)window->frontbuffer);

                    window->frontbuffer = new_frontbuffer;
                    window->frontbuffer_handle = flip_window.frontbuffer;
                }
                else
                {
                    logger_error("Client application gave us a jankie shared memory object id");
                }
            }

            if (window->backbuffer_handle != flip_window.backbuffer)
            {
                Bitmap *new_backbuffer = NULL;

                size_t size;
                if (shared_memory_include(
                        flip_window.backbuffer,
                        (uintptr_t *)&new_backbuffer,
                        &size) == SUCCESS)
                {
                    shared_memory_free((uintptr_t)window->backbuffer);

                    window->backbuffer = new_backbuffer;
                    window->backbuffer_handle = flip_window.backbuffer;
                }
                else
                {
                    logger_error("Client application gave us a jankie shared memory object id");
                }
            }

            renderer_region_dirty(rectangle_offset(flip_window.bound, window->bound.position));
            client_send_message(client, COMPOSITOR_MESSAGE_ACK, NULL, 0);
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", flip_window.id, client);
        }

        break;
    }
    case COMPOSITOR_MESSAGE_WINDOW_MOVE:
    {
        CompositorWindowMove move_window = {};
        connection_receive(connection, &move_window, sizeof(CompositorWindowMove));

        if (handle_has_error(connection))
        {
            client->disconnected = true;
            client_close_disconnected_clients();

            return;
        }

        Window *window = manager_get_window(client, move_window.id);

        if (window)
        {
            window_move(window, move_window.position);
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", move_window.id, client);
        }

        break;
    }
    case COMPOSITOR_MESSAGE_WINDOW_RESIZE:
    {
        CompositorWindowResize resize_window = {};
        connection_receive(connection, &resize_window, sizeof(CompositorWindowResize));

        if (handle_has_error(connection))
        {
            client->disconnected = true;
            client_close_disconnected_clients();

            return;
        }

        Window *window = manager_get_window(client, resize_window.id);

        if (window)
        {
            window_resize(window, resize_window.bound);
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", resize_window.id, client);
        }

        break;
    }
    case COMPOSITOR_MESSAGE_CURSOR_STATE_CHANGE:
    {
        CompositorCursorStateChange cursor = {};
        connection_receive(connection, &cursor, sizeof(CompositorCursorStateChange));

        if (handle_has_error(connection))
        {
            client->disconnected = true;
            client_close_disconnected_clients();

            return;
        }

        Window *window = manager_get_window(client, cursor.id);

        renderer_region_dirty(cursor_bound());

        if (window)
        {
            window->cursor_state = cursor.state;
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", cursor.id, client);
        }

        renderer_region_dirty(cursor_bound());

        break;
    }

    default:
        logger_warn("Invalid message for client %08x", client);
        break;
    }
}

static List *_connected_client = NULL;

Client *client_create(Connection *connection)
{
    if (!_connected_client)
    {
        _connected_client = list_create();
    }

    Client *client = __create(Client);

    client->connection = connection;
    client->notifier = notifier_create(
        client,
        HANDLE(connection),
        SELECT_READ,
        (NotifierCallback)client_request_callback);

    list_pushback(_connected_client, client);

    logger_info("Client %08x connected", client);

    return client;
}

IterationDecision destroy_window_if_client_match(Client *client, Window *window)
{
    if (window->client == client)
    {
        window_destroy(window);
    }

    return ITERATION_CONTINUE;
}

void client_close_all_windows(Client *client)
{
    list_iterate(manager_get_windows(), client, (ListIterationCallback)destroy_window_if_client_match);
}

void client_destroy(Client *client)
{
    logger_info("Disconnecting client %08x", client);

    client_close_all_windows(client);
    list_remove(_connected_client, client);
    notifier_destroy(client->notifier);
    connection_close(client->connection);
    free(client);
}

Result client_send_message(Client *client, CompositorMessageType type, const void *message, size_t size)
{
    if (client->disconnected)
    {
        return ERR_STREAM_CLOSED;
    }

    CompositorMessage header = {type, size};
    connection_send(client->connection, &header, sizeof(header));

    if (handle_has_error(client->connection))
    {
        logger_trace("Failled to send header to %08x", client);
        client->disconnected = true;
        return handle_get_error(client->connection);
    }

    if (!message)
        return SUCCESS;

    connection_send(client->connection, message, size);

    if (handle_has_error(client->connection))
    {
        logger_trace("Failled to send message to %08x", client);
        client->disconnected = true;
        return handle_get_error(client->connection);
    }

    return SUCCESS;
}

IterationDecision client_destroy_if_disconnected(void *target, Client *client)
{
    __unused(target);

    if (client->disconnected)
    {
        client_destroy(client);
    }

    return ITERATION_CONTINUE;
}

void client_close_disconnected_clients(void)
{
    if (_connected_client)
    {
        list_iterate(_connected_client, NULL, (ListIterationCallback)client_destroy_if_disconnected);
    }
}
