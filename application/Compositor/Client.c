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

    Message header;
    connection_receive(connection, &header);

    CompositorMessage *message = malloc(header.size);

    connection_payload(connection, (Message *)message);

    switch (message->type)
    {
    case COMPOSITOR_MESSAGE_CREATE_WINDOW:
    {
        CompositorCreateWindowMessage *create_window = (CompositorCreateWindowMessage *)message;
        Bitmap *bitmap = NULL;

        size_t size;
        shared_memory_include(create_window->framebuffer, (uintptr_t *)&bitmap, &size);
        window_create(create_window->id, client, create_window->bound, bitmap);
        break;
    }
    case COMPOSITOR_MESSAGE_DESTROY_WINDOW:
    {
        CompositorDestroyWindowMessage *destroy_window = (CompositorDestroyWindowMessage *)message;

        Window *window = manager_get_window(client, destroy_window->id);

        if (window)
        {
            window_destroy(window);
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", destroy_window->id, client);
        }

        break;
    }
    case COMPOSITOR_MESSAGE_BLIT_WINDOW:
    {
        CompositorBlitWindowMessage *blit_window = (CompositorBlitWindowMessage *)message;

        Window *window = manager_get_window(client, blit_window->id);

        if (window)
        {
            renderer_region_dirty(rectangle_offset(blit_window->bound, window->bound.position));
        }
        else
        {
            logger_warn("Invalid window id %d for client %08x", blit_window->id, client);
        }

        break;
    }

    default:
        logger_warn("Invalide message for client %08x", client);
        break;
    }

    free(message);
}

Client *client_create(Connection *connection)
{
    Client *client = __create(Client);

    client->connection = connection;
    notifier_initialize((Notifier *)client, HANDLE(connection), SELECT_RECEIVE);
    NOTIFIER(client)->on_ready_to_receive = (NotifierHandler)client_request_callback;

    return client;
}

void client_destroy(Client *client)
{
    notifier_uninitialize((Notifier *)client);
    connection_close(client->connection);
    free(client);
}