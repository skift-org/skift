#pragma once

#include <libipc/Peer.h>
#include <libsettings/Protocol.h>

namespace Settings
{

class ServerConnection : public IPC::Peer<Protocol>
{
public:
    Callback<void(const Path &path, const Json::Value &value)> on_notify;

    static OwnPtr<ServerConnection> open()
    {
        auto connection = IO::Socket::connect("/Session/settings.ipc");
        return own<ServerConnection>(connection.unwrap());
    }

    ServerConnection(IO::Connection connection) : Peer{move(connection)}
    {
    }

    ResultOr<Message> request(Message message, Message::Type expected)
    {
        return send_and_wait_for(message, [&](const Message &incoming) {
            return incoming.type == expected;
        });
    }

    void handle_message(const Message &message) override
    {
        if (message.type == Message::SERVER_NOTIFY)
        {
            on_notify(message.path.unwrap(), message.payload.unwrap());
        }
    }
};

} // namespace Settings
