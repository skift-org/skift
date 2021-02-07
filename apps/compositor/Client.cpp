#include <assert.h>
#include <libsystem/Logger.h>
#include <libsystem/system/Memory.h>
#include <libsystem/utils/Hexdump.h>

#include "compositor/Client.h"
#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Protocol.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static Vector<OwnPtr<Client>> _clients;

void Client::handle(const CompositorCreateWindow &create_window)
{
    if (manager_get_window(this, create_window.id))
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
               this,
               create_window.bound,
               frontbuffer.take_value(),
               backbuffer.take_value());
}

void Client::handle(const CompositorDestroyWindow &destroy_window)
{
    Window *window = manager_get_window(this, destroy_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", destroy_window.id, this);
        return;
    }

    delete window;
}

void Client::handle(const CompositorMoveWindow &move_window)
{
    Window *window = manager_get_window(this, move_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", move_window.id, this);
        return;
    }

    window->move(move_window.position);
}

void Client::handle(const CompositorFlipWindow &flip_window)
{
    Window *window = manager_get_window(this, flip_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", flip_window.id, this);
        return;
    }

    window->resize(flip_window.bound);
    window->flip_buffers(flip_window.frontbuffer, flip_window.frontbuffer_size, flip_window.backbuffer, flip_window.backbuffer_size, flip_window.dirty);

    CompositorMessage message = {};
    message.type = COMPOSITOR_MESSAGE_ACK;
    send_message(message);
}

void Client::handle(const CompositorCursorWindow &cursor_window)
{
    Window *window = manager_get_window(this, cursor_window.id);

    if (!window)
    {
        logger_warn("Invalid window id %d for client %08x", cursor_window.id, this);
        return;
    }

    renderer_region_dirty(cursor_dirty_bound());

    window->cursor_state(cursor_window.state);

    renderer_region_dirty(cursor_dirty_bound());
}

void Client::handle(const CompositorSetResolution &set_resolution)
{
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

void Client::handle_get_mouse_position()
{
    CompositorMessage message = {};
    message.type = COMPOSITOR_MESSAGE_MOUSE_POSITION;
    message.mouse_position.position = cursor_position();

    send_message(message);
}

void Client::handle_goodbye()
{
    _disconnected = true;

    CompositorMessage message = {};
    message.type = COMPOSITOR_MESSAGE_ACK;
    send_message(message);
}

void Client::handle_request()
{
    assert(!_disconnected);

    CompositorMessage message = {};
    size_t message_size = connection_receive(_connection, &message, sizeof(CompositorMessage));

    if (handle_has_error(_connection))
    {
        logger_error("Client handle has error: %s!", handle_error_string(_connection));

        _disconnected = true;
        client_destroy_disconnected();
        return;
    }

    if (message_size != sizeof(CompositorMessage))
    {
        logger_error("Got a message with an invalid size from client %u != %u!", sizeof(CompositorMessage), message_size);
        hexdump(&message, message_size);

        _disconnected = true;
        client_destroy_disconnected();
        return;
    }

    switch (message.type)
    {
    case COMPOSITOR_MESSAGE_CREATE_WINDOW:
        handle(message.create_window);
        break;

    case COMPOSITOR_MESSAGE_DESTROY_WINDOW:
        handle(message.destroy_window);
        break;

    case COMPOSITOR_MESSAGE_MOVE_WINDOW:
        handle(message.move_window);
        break;

    case COMPOSITOR_MESSAGE_FLIP_WINDOW:
        handle(message.flip_window);
        break;

    case COMPOSITOR_MESSAGE_CURSOR_WINDOW:
        handle(message.cursor_window);
        break;

    case COMPOSITOR_MESSAGE_SET_RESOLUTION:
        handle(message.set_resolution);
        break;

    case COMPOSITOR_MESSAGE_GET_MOUSE_POSITION:
        handle_get_mouse_position();
        break;

    case COMPOSITOR_MESSAGE_GOODBYE:
        handle_goodbye();
        break;

    default:
        logger_error("Invalid message for client %08x", this);
        hexdump(&message, message_size);

        _disconnected = true;
        client_destroy_disconnected();

        break;
    }
}

void Client::connect(Connection *connection)
{
    _clients.push_back(own<Client>(connection));
}

Client::Client(Connection *connection)
{
    _connection = connection;

    _notifier = own<Notifier>(HANDLE(connection), POLL_READ, [this]() {
        this->handle_request();
    });

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
    connection_close(_connection);
}

void client_broadcast(CompositorMessage message)
{
    _clients.foreach ([&](auto &client) {
        client->send_message(message);
        return Iteration::CONTINUE;
    });
}

Result Client::send_message(CompositorMessage message)
{
    if (_disconnected)
    {
        return ERR_STREAM_CLOSED;
    }

    connection_send(_connection, &message, sizeof(CompositorMessage));

    if (handle_has_error(_connection))
    {
        logger_error("Failed to send message to %08x: %s", this, handle_error_string(_connection));
        _disconnected = true;
        return handle_get_error(_connection);
    }

    return SUCCESS;
}

void client_destroy_disconnected()
{
    _clients.remove_all_match([](auto &client) { return client->_disconnected; });
}
