#pragma once

#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>

#include "compositor/Protocol.h"

struct Client
{
    Notifier *notifier = nullptr;
    Connection *connection = nullptr;
    bool disconnected = false;

    Client(Connection *connection);

    ~Client();

    Result send_message(CompositorMessage message);
};

void client_broadcast(CompositorMessage message);

void client_destroy_disconnected();
