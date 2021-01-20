#pragma once

#include <libutils/Vector.h>

#include <libsettings/ipc/Peer.h>

namespace settings
{

class ClientConnection : public Peer
{
private:
    Repository &_repository;
    Vector<Path> _observed_paths;

public:
    ClientConnection(Connection *connection, Repository &repository)
        : Peer(connection),
          _repository{repository}
    {
    }

    void handle_message(const Message &message) override
    {
        if (message.type == MessageType::CLIENT_READ)
        {
            Message response;

            response.type = MessageType::SERVER_VALUE;
            response.path = message.path.value();
            response.payload = _repository.read(message.path.value());

            send(response);
        }
        else if (message.type == MessageType::CLIENT_WRITE)
        {
            Message response;

            response.type = MessageType::SERVER_ACK;

            send(response);
        }
        else if (message.type == MessageType::CLIENT_WATCH)
        {
            _observed_paths.push_back(*message.path);

            Message response;
            response.type = MessageType::SERVER_ACK;
            send(response);
        }
        else if (message.type == MessageType::CLIENT_UNWATCH)
        {
            _observed_paths.remove_all_value(*message.path);

            Message response;
            response.type = MessageType::SERVER_ACK;
            send(response);
        }
    }

    void disconnected() override
    {
    }
};

} // namespace settings
