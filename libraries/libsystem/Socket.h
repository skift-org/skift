#pragma once

#include <abi/Handle.h>
#include <abi/Message.h>
#include <libsystem/error.h>
#include <libsystem/runtime.h>

typedef struct
{
    Handle handle;
} Socket;

Socket *socket_open(const char *path);

void socket_close(Socket *socket);

bool socket_send(Socket *socket, Message *message);

bool socket_receive(Socket *socket, Message *message);

bool socket_request(Socket *socket, Message *request, Message *respond);

bool socket_payload(Socket *socket, Message *payload);

bool socket_discard(Socket *sochet, Message *message);

bool socket_respond(Socket *socket, Message *respond);
