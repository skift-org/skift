#pragma once

#include <libipc/Peer.h>
#include <libsettings/Protocol.h>
#include <libutils/Vector.h>

namespace settings
{

class Client : public ipc::Peer<Protocol>
{
private:
    Vector<Path> _subscriptions;

public:
    Callback<void(Client &, const Message &message)> on_message;
    Callback<void()> on_disconnect;

    Client(Connection *connection) : Peer(connection)
    {
    }

    void handle_message(const Message &message) override
    {
        on_message(*this, message);
    }

    void handle_disconnect() override
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
