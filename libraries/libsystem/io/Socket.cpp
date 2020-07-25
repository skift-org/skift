
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>
#include <libsystem/utils/List.h>

struct Socket
{
    Handle handle;
    List *connections;
};

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
    Handle handle = {};
    __plug_handle_connect(&handle, path);
    return connection_create(nullptr, handle);
}

Connection *socket_accept(Socket *socket)
{
    Handle handle = {};
    __plug_handle_accept(HANDLE(socket), &handle);
    return connection_create(socket, handle);
}

void socket_did_connection_open(Socket *socket, struct Connection *connection)
{
    if (socket)
    {
        list_pushback(socket->connections, connection);
    }
}

void socket_did_connection_close(Socket *socket, struct Connection *connection)
{
    if (socket != nullptr)
    {
        list_remove(socket->connections, connection);
    }
}
