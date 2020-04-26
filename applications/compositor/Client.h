#pragma once

#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>

#include "compositor/Protocol.h"

typedef struct Client
{
    Notifier *notifier;
    Connection *connection;
    bool disconnected;
} Client;

Client *client_create(Connection *connection);

void client_destroy(Client *client);

Result client_send_message(Client *client, CompositorMessageType type, const void *message, size_t size);
