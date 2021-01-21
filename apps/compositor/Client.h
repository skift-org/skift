#pragma once

#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>

#include "compositor/Protocol.h"

struct Client
{
    OwnPtr<Notifier> _notifier = nullptr;
    Connection *_connection = nullptr;
    bool _disconnected = false;

    static void connect(Connection *connection);

    Client(Connection *connection);

    ~Client();

    Result send_message(CompositorMessage message);

    void handle(const CompositorCreateWindow &create_window);

    void handle(const CompositorDestroyWindow &destroy_window);

    void handle(const CompositorMoveWindow &move_window);

    void handle(const CompositorFlipWindow &flip_window);

    void handle(const CompositorCursorWindow &cursor_window);

    void handle(const CompositorSetResolution &set_resolution);

    void handle_get_mouse_position();

    void handle_goodbye();

    void handle_request();
};

void client_broadcast(CompositorMessage message);

void client_destroy_disconnected();
