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

        Bitmap *bitmap = NULL;

        size_t size;
        shared_memory_include(create_window.framebuffer, (uintptr_t *)&bitmap, &size);
        window_create(create_window.id, client, create_window.bound, bitmap);
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
            shared_memory_free((uintptr_t)window->framebuffer);
            window->framebuffer = NULL;
            window_destroy(window);
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", destroy_window.id, client);
        }

        break;
    }
    case COMPOSITOR_MESSAGE_BLIT_WINDOW:
    {
        CompositorBlitWindowMessage blit_window = {};
        connection_receive(connection, &blit_window, sizeof(CompositorBlitWindowMessage));

        if (handle_has_error(connection))
        {
            client->disconnected = true;
            client_close_disconnected_clients();

            return;
        }

        Window *window = manager_get_window(client, blit_window.id);

        if (window)
        {
            int old_framebuffer_handle = 0;
            shared_memory_get_handle((uintptr_t)window->framebuffer, &old_framebuffer_handle);

            if (old_framebuffer_handle != blit_window.framebuffer)
            {
                Bitmap *new_framebuffer = NULL;

                size_t size;
                if (shared_memory_include(
                        blit_window.framebuffer,
                        (uintptr_t *)&new_framebuffer,
                        &size) == SUCCESS)
                {
                    logger_info("Flipping window framebuffer");
                    shared_memory_free((uintptr_t)window->framebuffer);

                    window->framebuffer = new_framebuffer;
                }
                else
                {
                    logger_error("Client application gave us a jankie shared memory object id");
                }
            }

            renderer_region_dirty(rectangle_offset(blit_window.bound, window->bound.position));
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", blit_window.id, client);
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
        logger_warn("Invalide message for client %08x", client);
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
