#pragma once

#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Socket.h>

#include <libsettings/storage/Repository.h>

#include "settings-service/ipc/ClientConnection.h"

namespace settings
{

class Server;

void server_accept_callback(Server *target, Socket *socket, PollEvent events);

class Server
{
private:
    Socket *_socket;
    OwnPtr<Notifier> _notifier;

    Vector<OwnPtr<ClientConnection>> _clients{};
    Repository &_repository;

public:
    Server(Repository &repository)
        : _repository(repository)
    {
        _socket = socket_open("/Session/settings.ipc", OPEN_CREATE);

        _notifier = own<Notifier>(HANDLE(_socket), POLL_ACCEPT, [this]() {
            handle_connection(socket_accept(_socket));
        });
    }

    ~Server()
    {
        socket_close(_socket);
    }

    void handle_connection(Connection *connection)
    {
        auto client = own<ClientConnection>(connection, _repository);
        _clients.push_back(own<ClientConnection>(client));
    }
};

} // namespace settings
