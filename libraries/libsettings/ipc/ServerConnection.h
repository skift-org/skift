#pragma once

#include <libsettings/ipc/Peer.h>

namespace settings
{

class ServerConnection : public Peer
{
public:
    ServerConnection(Connection *connection) : Peer(connection)
    {
    }

    static OwnPtr<ServerConnection>
    open()
    {
        return own<ServerConnection>(socket_connect("/Session/"));
    }
};

} // namespace settings
