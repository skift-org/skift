#include <libsettings/ServerConnection.h>
#include <libsettings/Service.h>
#include <libsettings/Watcher.h>
#include <libsystem/Logger.h>
#include <libutils/Vector.h>

namespace Settings
{

static RefPtr<Service> _instance = nullptr;

RefPtr<Service> Service::the()
{
    if (_instance == nullptr)
    {
        _instance = make<Service>();
    }

    return _instance;
}

void Service::notify_watchers(const Path &path, const Json::Value &value)
{
    for (size_t i = 0; i < _watchers.count(); i++)
    {
        if (path.match(_watchers[i]->path()))
        {
            _watchers[i]->invoke(value);
        }
    }
}

ServerConnection &Service::server()
{
    if (!_server)
    {
        _server = ServerConnection::open();
        _server->on_notify = [this](const Path &path, const Json::Value &value) {
            notify_watchers(path, value);
        };
    }

    return *_server;
}

bool Service::is_watching_path(const Path &path)
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

void Service::register_watcher(Watcher &watcher)
{
    if (!is_watching_path(watcher.path()))
    {
        Message message;

        message.type = Message::CLIENT_WATCH;
        message.path = watcher.path();

        server().request(message, Message::SERVER_ACK);
    }

    _watchers.push_back(&watcher);
}

void Service::unregister_watcher(Watcher &watcher)
{
    _watchers.remove_value(&watcher);

    if (!is_watching_path(watcher.path()))
    {
        Message message;

        message.type = Message::CLIENT_UNWATCH;
        message.path = watcher.path();

        server().request(message, Message::SERVER_ACK);
    }
}

Optional<Json::Value> Service::read(const Path path)
{
    Message message;

    message.type = Message::CLIENT_READ;
    message.path = path;

    auto result_or_response = server().request(message, Message::SERVER_VALUE);

    if (!result_or_response.success())
    {
        return {};
    }

    return result_or_response.unwrap().payload;
}

bool Service::write(const Path path, Json::Value value)
{
    Message message;

    message.type = Message::CLIENT_WRITE;
    message.path = path;
    message.payload = value;

    auto result = server().request(message, Message::SERVER_ACK);

    if (result.success())
    {
        notify_watchers(path, value);
    }

    return result.success();
}

} // namespace Settings