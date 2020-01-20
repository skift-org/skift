#include <libsystem/Socket.h>
#include <libsystem/__plugs__.h>

Socket *socket_open(const char *path)
{
    Socket *socket = __create(Socket);

    __plug_handle_open(HANDLE(socket), path, OPEN_SOCKET);

    return socket;
}

void socket_close(Socket *socket)
{
    __plug_handle_close(HANDLE(socket));

    free(socket);
}

bool socket_send(Socket *socket, Message *message)
{
    __plug_handle_send(HANDLE(socket), message);

    return !handle_has_error(socket);
}

bool socket_receive(Socket *socket, Message *message)
{
    __plug_handle_receive(HANDLE(socket), message);

    return !handle_has_error(socket);
}

bool socket_request(Socket *socket, Message *request, Message *respond)
{
    __plug_handle_request(HANDLE(socket), request, respond);

    return !handle_has_error(socket);
}

bool socket_payload(Socket *socket, Message *payload)
{
    __plug_handle_payload(HANDLE(socket), payload);

    return !handle_has_error(socket);
}

bool socket_discard(Socket *socket)
{
    __plug_handle_discard(HANDLE(socket));

    return !handle_has_error(socket);
}

bool socket_respond(Socket *socket, Message *respond)
{
    __plug_handle_respond(HANDLE(socket), respond);

    return !handle_has_error(socket);
}