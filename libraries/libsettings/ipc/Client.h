#pragma once

#include <libsettings/ipc/ServerConnection.h>

namespace settings
{

class Client
{
private:
    OwnPtr<ServerConnection> _server;

public:
    Client()
    {
        _server = ServerConnection::open();
    }

    ~Client()
    {
    }

    void watch(const Path &path)
    {
        Message message;

        message.type = MessageType::CLIENT_WATCH;
        message.path = path;

        _server->send(message);
    }

    void unwatch(const Path &path)
    {
        Message message;

        message.type = MessageType::CLIENT_UNWATCH;
        message.path = path;

        _server->send(message);
    }

    ResultOr<json::Value> read(const Path &path)
    {
        Message message;

        message.type = MessageType::CLIENT_READ;
        message.path = path;

        auto result_or_response = _server->request(message, MessageType::SERVER_VALUE);

        if (!result_or_response)
        {
            return result_or_response.result();
        }

        return *result_or_response->payload;
    }

    bool write(const Path &path, const json::Value &value)
    {
        Message message;

        message.type = MessageType::CLIENT_READ;
        message.path = path;
        message.payload = value;

        auto result = _server->request(message, MessageType::SERVER_ACK);

        return result.success();
    }
};

} // namespace settings
