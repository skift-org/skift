#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/system/Memory.h>
#include <libsystem/utils/Hexdump.h>

#include "compositor/Client.h"
#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Protocol.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static List *_connected_client = nullptr;

void client_handle_create_window(Client *client, CompositorCreateWindow create_window)
{
    if (manager_get_window(client, create_window.id))
    {
        logger_error("Duplicated window id %d!", create_window.id);
        return;
    }

    auto frontbuffer = Bitmap::create_shared_from_handle(create_window.frontbuffer, create_window.backbuffer_size);

    if (!frontbuffer.success())
    {
        return;
    }

    auto backbuffer = Bitmap::create_shared_from_handle(create_window.backbuffer, create_window.backbuffer_size);

    if (!backbuffer.success())
    {
        return;
    }

    new Window(create_window.id,
               create_window.flags,
               create_window.type,
               client,
               create_window.bound,
               frontbuffer.take_value(),
               backbuffer.take_value());
}

void client_handle_destroy_window(Client *client, CompositorDestroyWindow destroy_window)
{
    Window *window = manager_get_window(client, destroy_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", destroy_window.id, client);
        return;
    }

    delete window;
}

void client_handle_resize_window(Client *client, CompositorResizeWindow resize_window)
{
    Window *window = manager_get_window(client, resize_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", resize_window.id, client);
        return;
    }

    window->resize(resize_window.bound);
}

void client_handle_move_window(Client *client, CompositorMoveWindow move_window)
{
    Window *window = manager_get_window(client, move_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", move_window.id, client);
        return;
    }

    window->move(move_window.position);
}

void client_handle_flip_window(Client *client, CompositorFlipWindow flip_window)
{
    Window *window = manager_get_window(client, flip_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", flip_window.id, client);
        return;
    }

    window->flip_buffers(flip_window.frontbuffer, flip_window.frontbuffer_size, flip_window.backbuffer, flip_window.backbuffer_size, flip_window.bound);

    CompositorMessage message = {};
    message.type = COMPOSITOR_MESSAGE_ACK;
    client->send_message(message);
}

void client_handle_cursor_window(Client *client, CompositorCursorWindow cursor_window)
{
    Window *window = manager_get_window(client, cursor_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", cursor_window.id, client);
        return;
    }

    renderer_region_dirty(cursor_dirty_bound());

    window->cursor_state(cursor_window.state);

    renderer_region_dirty(cursor_dirty_bound());
}

void client_handle_set_resolution(Client *client, CompositorSetResolution set_resolution)
{
    __unused(client);

    if (renderer_set_resolution(set_resolution.width, set_resolution.height))
    {
        client_broadcast((CompositorMessage){
            .type = COMPOSITOR_MESSAGE_CHANGED_RESOLUTION,
            .changed_resolution = {
                .resolution = renderer_bound(),
            },
        });
    }
}

void client_handle_set_wallpaper(Client *client, CompositorSetWallaper set_wallpaper)
{
    __unused(client);

    auto wallaper = Bitmap::create_shared_from_handle(set_wallpaper.wallpaper, set_wallpaper.resolution);

    if (wallaper.success())
    {
        renderer_set_wallaper(wallaper.take_value());
    }
    else
    {
        logger_warn("The client sent us a bad wallpaper handle.");
    }
}

void client_handle_get_mouse_position(Client *client)
{
    CompositorMessage message = {};
    message.type = COMPOSITOR_MESSAGE_MOUSE_POSITION;
    message.mouse_position.position = cursor_position();

    client->send_message(message);
}

void client_request_callback(Client *client, Connection *connection, PollEvent events)
{
    assert(events & POLL_READ);

    CompositorMessage message = {};
    size_t message_size = connection_receive(connection, &message, sizeof(CompositorMessage));

    if (handle_has_error(connection))
    {
        logger_error("Client handle has error: %s!", handle_error_string(connection));

        client->disconnected = true;
        client_destroy_disconnected();
        return;
    }

    if (message_size != sizeof(CompositorMessage))
    {
        logger_error("Got a message with an invalid size from client %u != %u!", sizeof(CompositorMessage), message_size);
        hexdump(&message, message_size);

        client->disconnected = true;
        client_destroy_disconnected();

        return;
    }

    switch (message.type)
    {
    case COMPOSITOR_MESSAGE_CREATE_WINDOW:
        client_handle_create_window(client, message.create_window);
        break;

    case COMPOSITOR_MESSAGE_DESTROY_WINDOW:
        client_handle_destroy_window(client, message.destroy_window);
        break;

    case COMPOSITOR_MESSAGE_RESIZE_WINDOW:
        client_handle_resize_window(client, message.resize_window);
        break;

    case COMPOSITOR_MESSAGE_MOVE_WINDOW:
        client_handle_move_window(client, message.move_window);
        break;

    case COMPOSITOR_MESSAGE_FLIP_WINDOW:
        client_handle_flip_window(client, message.flip_window);
        break;

    case COMPOSITOR_MESSAGE_CURSOR_WINDOW:
        client_handle_cursor_window(client, message.cursor_window);
        break;

    case COMPOSITOR_MESSAGE_SET_RESOLUTION:
        client_handle_set_resolution(client, message.set_resolution);
        break;

    case COMPOSITOR_MESSAGE_SET_WALLPAPER:
        client_handle_set_wallpaper(client, message.set_wallaper);
        break;

    case COMPOSITOR_MESSAGE_GET_MOUSE_POSITION:
        client_handle_get_mouse_position(client);
        break;

    default:
        logger_error("Invalid message for client %08x", client);
        hexdump(&message, message_size);

        client->disconnected = true;
        client_destroy_disconnected();

        break;
    }
}

Client::Client(Connection *connection)
{
    if (!_connected_client)
    {
        _connected_client = list_create();
    }

    this->connection = connection;
    this->notifier = notifier_create(
        this,
        HANDLE(connection),
        POLL_READ,
        (NotifierCallback)client_request_callback);

    list_pushback(_connected_client, this);

    logger_info("Client %08x connected", this);

    this->send_message((CompositorMessage){
        .type = COMPOSITOR_MESSAGE_GREETINGS,
        .greetings = {
            .screen_bound = renderer_bound(),
        },
    });
}

Iteration destroy_window_if_client_match(Client *client, Window *window)
{
    if (window->client() == client)
    {
        delete window;
    }

    return Iteration::CONTINUE;
}

void client_close_all_windows(Client *client)
{
    list_iterate(manager_get_windows(), client, (ListIterationCallback)destroy_window_if_client_match);
}

Client::~Client()
{
    logger_info("Disconnecting client %08x", this);

    client_close_all_windows(this);
    list_remove(_connected_client, this);
    notifier_destroy(notifier);
    connection_close(connection);
}

void client_broadcast(CompositorMessage message)
{
    list_foreach(Client, client, _connected_client)
    {
        client->send_message(message);
    }
}

Result Client::send_message(CompositorMessage message)
{
    if (disconnected)
    {
        return ERR_STREAM_CLOSED;
    }

    connection_send(connection, &message, sizeof(CompositorMessage));

    if (handle_has_error(connection))
    {
        logger_error("Failed to send message to %08x: %s", this, handle_error_string(connection));
        disconnected = true;
        return handle_get_error(connection);
    }

    return SUCCESS;
}

Iteration client_destroy_if_disconnected(void *target, Client *client)
{
    __unused(target);

    if (client->disconnected)
    {
        delete client;
    }

    return Iteration::CONTINUE;
}

void client_destroy_disconnected()
{
    if (_connected_client)
    {
        list_iterate(_connected_client, nullptr, (ListIterationCallback)client_destroy_if_disconnected);
    }
}
