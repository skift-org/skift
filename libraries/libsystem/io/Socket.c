/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>

Socket *socket_open(const char *path, OpenFlag flags)
{
    Socket *socket = __create(Socket);

    __plug_handle_open(HANDLE(socket), path, flags | OPEN_SOCKET);

    socket->connections = list_create();

    return socket;
}

void socket_close(Socket *socket)
{
    list_destroy_with_callback(socket->connections, (ListDestroyElementCallback)connection_close);
    __plug_handle_close(HANDLE(socket));
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
