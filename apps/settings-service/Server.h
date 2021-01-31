#pragma once

#include <libsystem/eventloop/Invoker.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Socket.h>

#include "settings-service/Client.h"
#include "settings-service/Repository.h"

namespace settings
{

class Server
{
private:
    Socket *_socket;
    OwnPtr<Notifier> _notifier;
    OwnPtr<Invoker> _invoker;

    Vector<OwnPtr<Client>> _clients{};
    Repository &_repository;

public:
    Server(Repository &repository) : _repository(repository)
    {
        _socket = socket_open("/Session/settings.ipc", OPEN_CREATE);

        _notifier = own<Notifier>(HANDLE(_socket), POLL_ACCEPT, [this]() {
            auto connection = socket_accept(_socket);

            auto client = own<Client>(connection);

            client->on_message = [this](auto &client, auto &message) {
                handle_client_message(client, message);
            };

            client->on_disconnect = [this]() {
                handle_client_disconnected();
            };

            _clients.push_back(client);
        });

        _invoker = own<Invoker>([this]() {
            _clients.remove_all_match([](auto &client) {
                return !client->connected();
            });
        });
    }

    void handle_client_message(Client &client, const Message &message)
    {
        if (message.type == Message::CLIENT_READ)
        {
            Message response;
            response.type = Message::SERVER_VALUE;
            response.path = *message.path;
            response.payload = _repository.read(*message.path);

            client.send(response);
        }
        else if (message.type == Message::CLIENT_WRITE)
        {
            _repository.write(*message.path, *message.payload);

            for (size_t i = 0; i < _clients.count(); i++)
            {
                if (_clients[i] != &client &&
                    _clients[i]->is_subscribe(*message.path))
                {
                    Message notification;
                    notification.type = Message::SERVER_NOTIFY;
                    notification.path = *message.path;
                    notification.payload = _repository.read(*message.path);

                    _clients[i]->send(notification);
                }
            }

            Message response;
            response.type = Message::SERVER_ACK;

            client.send(response);
        }
        else if (message.type == Message::CLIENT_WATCH)
        {
            client.subscribe(*message.path);

            Message response;
            response.type = Message::SERVER_ACK;

            client.send(response);
        }
        else if (message.type == Message::CLIENT_UNWATCH)
        {
            client.unsubscribe(*message.path);

            Message response;
            response.type = Message::SERVER_ACK;

            client.send(response);
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
