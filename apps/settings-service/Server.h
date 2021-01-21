#pragma once

#include <libsystem/eventloop/Invoker.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Socket.h>

#include <libsettings/Repository.h>

#include "settings-service/ClientConnection.h"

namespace settings
{

class Server
{
private:
    Socket *_socket;
    OwnPtr<Notifier> _notifier;
    OwnPtr<Invoker> _invoker;

    Vector<OwnPtr<ClientConnection>> _clients{};
    Repository &_repository;

public:
    Server(Repository &repository) : _repository(repository)
    {
        _socket = socket_open("/Session/settings.ipc", OPEN_CREATE);

        _notifier = own<Notifier>(HANDLE(_socket), POLL_ACCEPT, [this]() {
            auto connection = socket_accept(_socket);

            auto client = own<ClientConnection>(connection);

            client->on_message = [this](auto &client, auto &message) {
                handle_client_message(client, message);
            };

            client->on_disconnect = [this]() {
                handle_client_disconnected();
            };

            _clients.push_back(client);
        });

        _invoker = own<Invoker>([this]() {
            _clients.remove_all_match([](auto& client) {
                return !client->connected();
            });
        });
    }

    void handle_client_message(ClientConnection &client, const Message &message)
    {
        if (message.type == MessageType::CLIENT_READ)
        {
            Message r;

            r.type = MessageType::SERVER_VALUE;
            r.path = *message.path;
            r.payload = _repository.read(*message.path);

            client.send(r);
        }
        else if (message.type == MessageType::CLIENT_WRITE)
        {
            _repository.write(*message.path, *message.payload);

            for (size_t i = 0; i < _clients.count(); i++)
            {
                if (_clients[i] != &client &&
                    _clients[i]->is_subscribe(*message.path))
                {
                    Message m;

                    m.type = MessageType::SERVER_NOTIFY;
                    m.path = *message.path;
                    m.payload = _repository.read(*message.path);

                    _clients[i]->send(m);
                }
            }

            Message r;
            r.type = MessageType::SERVER_ACK;
            client.send(r);
        }
        else if (message.type == MessageType::CLIENT_WATCH)
        {
            client.subscribe(*message.path);

            Message r;
            r.type = MessageType::SERVER_ACK;
            client.send(r);
        }
        else if (message.type == MessageType::CLIENT_UNWATCH)
        {
            client.unsubscribe(*message.path);

            Message r;
            r.type = MessageType::SERVER_ACK;
            client.send(r);
        }
        else
        {
            logger_warn("Unknown message %d!", message.type);
        }
    }

    void handle_client_disconnected()
    {
        _invoker->invoke_later();
    }

    ~Server()
    {
        socket_close(_socket);
    }
};

} // namespace settings
