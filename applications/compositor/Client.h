#pragma once

#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>

#include "compositor/Protocol.h"

struct Client
{
    Notifier *notifier;
    Connection *connection;
    bool disconnected;
};

Client *client_create(Connection *connection);

void client_destroy(Client *client);

Result client_send_message(Client *client, CompositorMessage message);

void client_destroy_disconnected();
