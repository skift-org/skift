#pragma once

#include <libsettings/ipc/Peer.h>

namespace settings
{

class ServerConnection : public Peer
{
public:
    Callback<void(const Path &path, const json::Value &value)> on_notify;

    ServerConnection(Connection *connection) : Peer(connection)
    {
    }

    static OwnPtr<ServerConnection>
    open()
    {
        return own<ServerConnection>(socket_connect("/Session/settings.ipc"));
    }

    void handle_message(const Message &message) override
    {
        if (message.type == MessageType::SERVER_NOTIFY)
        {
            on_notify(*message.path, *message.payload);
        }
    }
};

} // namespace settings
