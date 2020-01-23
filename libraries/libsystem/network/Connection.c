#include <libsystem/__plugs__.h>
#include <libsystem/assert.h>
#include <libsystem/network/Connection.h>
#include <libsystem/network/Socket.h>

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

void connection_send(Connection *connection, Message *message)
{
    assert(connection != NULL);
    assert(message != NULL);

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

bool connection_discard(Connection *connection)
{
    assert(connection != NULL);

    __plug_handle_discard(HANDLE(connection));
}