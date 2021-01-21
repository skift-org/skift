#include <libutils/Vector.h>

#include <libsettings/Settings.h>
#include <libsettings/Watcher.h>
#include <libsettings/ipc/Client.h>

namespace settings
{

static Vector<Watcher *> _watchers;
static OwnPtr<ServerConnection> _server;

static ServerConnection &server()
{
    if (!_server)
    {
        _server = ServerConnection::open();

        _server->on_notify = [](const Path &path, const json::Value &value) {
            for (size_t i = 0; i < _watchers.count(); i++)
            {
                if (path.match(_watchers[i]->path()))
                {
                    _watchers[i]->invoke(value);
                }
            }
        };
    }

    return *_server;
}

static bool is_watching_path(const Path &path)
{
    for (size_t i = 0; i < _watchers.count(); i++)
    {
        if (path.match(_watchers[i]->path()))
        {
            return true;
        }
    }

    return false;
}

void register_watcher(Watcher &watcher)
{
    if (!is_watching_path(watcher.path()))
    {
        Message message;

        message.type = MessageType::CLIENT_WATCH;
        message.path = watcher.path();

        server().request(message, MessageType::SERVER_ACK);
    }

    _watchers.push_back(&watcher);
}

void unregister_watcher(Watcher &watcher)
{
    _watchers.remove_value(&watcher);

    if (!is_watching_path(watcher.path()))
    {
        Message message;

        message.type = MessageType::CLIENT_UNWATCH;
        message.path = watcher.path();

        server().request(message, MessageType::SERVER_ACK);
    }
}

Optional<json::Value> read(const Path path)
{
    Message message;

    message.type = MessageType::CLIENT_READ;
    message.path = path;

    auto result_or_response = server().request(message, MessageType::SERVER_VALUE);

    if (!result_or_response)
    {
        return {};
    }

    return result_or_response->payload;
}

bool write(const Path path, json::Value value)
{
    Message message;

    message.type = MessageType::CLIENT_WRITE;
    message.path = path;
    message.payload = value;

    auto result = server().request(message, MessageType::SERVER_ACK);

    return result.success();
}

} // namespace settings