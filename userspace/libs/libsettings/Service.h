#pragma once

#include <libjson/Json.h>
#include <libsettings/Path.h>
#include <libsettings/ServerConnection.h>
#include <libutils/Opt.h>

namespace Settings
{

struct Watcher;

struct Service : public Shared<Service>
{
private:
    Vec<Watcher *> _watchers;
    Box<ServerConnection> _server;

public:
    // FIXME: Move this to an injection container.
    static Ref<Service> the();

    ServerConnection &server();

    Service() {}

    ~Service() {}

    void notify_watchers(const Path &path, const Json::Value &value);

    bool is_watching_path(const Path &path);

    void register_watcher(Watcher &watcher);

    void unregister_watcher(Watcher &watcher);

    Opt<Json::Value> read(const Path path);

    bool write(const Path path, Json::Value value);
};

} // namespace Settings
