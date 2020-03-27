#include <libsystem/logger.h>
#include <libsystem/memory.h>

#include "Compositor/Client.h"
#include "Compositor/Manager.h"
#include "Compositor/Protocol.h"
#include "Compositor/Renderer.h"
#include "Compositor/Window.h"

void client_request_callback(Client *client, Connection *connection)
{
    __unused(client);

    CompositorMessage header = {};

    connection_receive(connection, &header, sizeof(CompositorMessage));

    switch (header.type)
    {
    case COMPOSITOR_MESSAGE_CREATE_WINDOW:
    {
        CompositorCreateWindowMessage create_window = {};
        connection_receive(connection, &create_window, sizeof(CompositorCreateWindowMessage));

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

        Window *window = manager_get_window(client, destroy_window.id);

        if (window)
        {
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

        Window *window = manager_get_window(client, blit_window.id);

        if (window)
        {
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

    default:
        logger_warn("Invalide message for client %08x", client);
        break;
    }
}

Client *client_create(Connection *connection)
{
    Client *client = __create(Client);

    client->connection = connection;
    notifier_initialize((Notifier *)client, HANDLE(connection), SELECT_READ);
    NOTIFIER(client)->on_ready_to_read = (NotifierHandler)client_request_callback;

    return client;
}

void client_send_message(Client *client, CompositorMessageType type, const void *message, size_t size)
{
    CompositorMessage header = {type, size};
    connection_send(client->connection, &header, sizeof(header));
    connection_send(client->connection, message, size);
}

void client_destroy(Client *client)
{
    notifier_uninitialize((Notifier *)client);
    connection_close(client->connection);
    free(client);
}