/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/assert.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>

void connection_close(Connection *connection)
{
    assert(connection != NULL);

    if (connection->socket != NULL)
    {
        list_remove(connection->socket->connections, connection);
        connection->socket = NULL;
    }

    __plug_handle_close(HANDLE(connection));
}

void connection_send(Connection *connection, Message *message, size_t size)
{
    assert(connection != NULL);
    assert(message != NULL);

    message->size = size;

    __plug_handle_send(HANDLE(connection), message);
}

void connection_receive(Connection *connection, Message *message)
{
    assert(connection != NULL);
    assert(message != NULL);

    __plug_handle_receive(HANDLE(connection), message);
}

void connection_payload(Connection *connection, Message *message)
{
    assert(connection != NULL);
    assert(message != NULL);

    __plug_handle_payload(HANDLE(connection), message);
}

void connection_discard(Connection *connection)
{
    assert(connection != NULL);

    __plug_handle_discard(HANDLE(connection));
}