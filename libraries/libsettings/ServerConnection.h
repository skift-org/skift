#pragma once

#include <libipc/Peer.h>
#include <libsettings/Protocol.h>

namespace settings
{

class ServerConnection : public ipc::Peer<Protocol>
{
public:
    Callback<void(const Path &path, const json::Value &value)> on_notify;

    static OwnPtr<ServerConnection> open()
    {
        return own<ServerConnection>(socket_connect("/Session/settings.ipc"));
    }

    ServerConnection(Connection *connection) : Peer(connection)
    {
    }

    ResultOr<Message> request(Message message, Message::Type expected)
    {
        return send_and_wait_for(message, [&](const Message &incoming) { return incoming.type == expected; });
    }

    void handle_message(const Message &message) override
    {
        if (message.type == Message::SERVER_NOTIFY)
        {
            on_notify(*message.path, *message.payload);
        }
    }
};

} // namespace settings
