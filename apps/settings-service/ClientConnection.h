#pragma once

#include <libutils/Vector.h>

#include <libsettings/Repository.h>
#include <libsettings/ipc/Peer.h>

namespace settings
{

class ClientConnection : public Peer
{
private:
    Vector<Path> _subscriptions;

public:
    Callback<void(ClientConnection &, const Message &message)> on_message;
    Callback<void()> on_disconnect;

    ClientConnection(Connection *connection) : Peer(connection)
    {
    }

    void handle_message(const Message &message) override
    {
        on_message(*this, message);
    }

    void disconnected() override
    {
        on_disconnect();
    }

    void subscribe(const settings::Path &path)
    {
        _subscriptions.push_back(path);
    }

    void unsubscribe(const settings::Path &path)
    {
        _subscriptions.remove_all_value(path);
    }

    bool is_subscribe(const settings::Path &path)
    {
        for (size_t i = 0; i < _subscriptions.count(); i++)
        {
            if (path.match(_subscriptions[i]))
            {
                return true;
            }
        }

        return false;
    }
};

} // namespace settings
