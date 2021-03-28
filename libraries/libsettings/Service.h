#pragma once

#include <libsettings/Path.h>
#include <libsettings/ServerConnection.h>
#include <libutils/Optional.h>
#include <libutils/json/Json.h>

namespace Settings
{

class Watcher;

class Service : public RefCounted<Service>
{
private:
    Vector<Watcher *> _watchers;
    OwnPtr<ServerConnection> _server;

public:
    // FIXME: Move this to an injection container.
    static RefPtr<Service> the();

    ServerConnection &server();

    Service() {}

    ~Service() {}

    void notify_watchers(const Path &path, const Json::Value &value);

    bool is_watching_path(const Path &path);

    void register_watcher(Watcher &watcher);

    void unregister_watcher(Watcher &watcher);

    Optional<Json::Value> read(const Path path);

    bool write(const Path path, Json::Value value);
};

} // namespace Settings
