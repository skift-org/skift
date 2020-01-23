#include <libsystem/__plugs__.h>

#include <libsystem/network/Connection.h>
#include <libsystem/network/Socket.h>

Socket *socket_open(const char *path, OpenFlag flags)
{
    Socket *socket = __create(Socket);

    __plug_handle_open(HANDLE(socket), path, flags | OPEN_SOCKET);

    return socket;
}

Socket *socket_close(Socket *socket)
{
    Connection *connection;
    while (list_peek(socket->connections, (void **)&connection))
    {
        connection_close(connection);
    }

    __plug_handle_close(HANDLE(socket));

    list_destroy(socket->connections, LIST_KEEP_VALUES);
    free(socket);
}

Connection *socket_connect(const char *path)
{
    Connection *connection = __create(Connection);

    __plug_handle_connect(HANDLE(connection), path);

    connection->socket = NULL;

    return connection;
}

Connection *socket_accept(Socket *socket)
{
    Connection *connection = __create(Connection);

    __plug_handle_accept(HANDLE(socket), HANDLE(connection));

    list_pushback(socket->connections, connection);
    connection->socket = socket;

    return connection;
}
