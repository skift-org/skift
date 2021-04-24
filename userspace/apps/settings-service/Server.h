#pragma once

#include <libasync/Invoker.h>
#include <libasync/Notifier.h>
#include <libio/Socket.h>

#include "settings-service/Client.h"
#include "settings-service/Repository.h"

namespace Settings
{

class Server
{
private:
    IO::Socket _socket;
    OwnPtr<Async::Notifier> _notifier;
    OwnPtr<Async::Invoker> _invoker;

    Vector<OwnPtr<Client>> _clients{};
    Repository &_repository;

public:
    Server(Repository &repository) : _repository(repository)
    {
        _socket = IO::Socket{"/Session/settings.ipc", OPEN_CREATE};

        _notifier = own<Async::Notifier>(_socket, POLL_ACCEPT, [this]() {
            auto connection = _socket.accept();

            if (!connection.success())
            {
                return;
            }

            auto client = own<Client>(connection.unwrap());

            client->on_message = [this](auto &client, auto &message) {
                handle_client_message(client, message);
            };

            client->on_disconnect = [this]() {
                handle_client_disconnected();
            };

            _clients.push_back(client);
        });

        _invoker = own<Async::Invoker>([this]() {
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
            response.path = message.path;
            response.payload = _repository.read(message.path.unwrap());

            client.send(response);
        }
        else if (message.type == Message::CLIENT_WRITE)
        {
            _repository.write(message.path.unwrap(), message.payload.unwrap());

            for (size_t i = 0; i < _clients.count(); i++)
            {
                if (_clients[i] != &client &&
                    _clients[i]->is_subscribe(message.path.unwrap()))
                {
                    Message notification;
                    notification.type = Message::SERVER_NOTIFY;
                    notification.path = message.path;
                    notification.payload = _repository.read(message.path.unwrap());

                    _clients[i]->send(notification);
                }
            }

            Message response;
            response.type = Message::SERVER_ACK;

            client.send(response);
        }
        else if (message.type == Message::CLIENT_WATCH)
        {
            client.subscribe(message.path.unwrap());

            Message response;
            response.type = Message::SERVER_ACK;

            client.send(response);
        }
        else if (message.type == Message::CLIENT_UNWATCH)
        {
            client.unsubscribe(message.path.unwrap());

            Message response;
            response.type = Message::SERVER_ACK;

            client.send(response);
        }
        else
        {
            IO::logln("Unknown message {}!", message.type);
        }
    }

    void handle_client_disconnected()
    {
        _invoker->invoke_later();
    }
};

} // namespace Settings
