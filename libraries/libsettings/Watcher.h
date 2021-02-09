#pragma once

#include <libutils/Callback.h>

#include <libsettings/Path.h>
#include <libsettings/Settings.h>

namespace Settings
{

using WatcherCallback = Callback<void(const json::Value &)>;

class Watcher
{
private:
    Path _path;
    WatcherCallback _callback;

public:
    const Path &path() const
    {
        return _path;
    }

    Watcher(Path path, WatcherCallback callback)
        : _path{path}, _callback{callback}
    {
        register_watcher(*this);
    }

    ~Watcher()
    {
        unregister_watcher(*this);
    }

    void invoke(const json::Value &value)
    {
        _callback(value);
    }
};

} // namespace Settings
