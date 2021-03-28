#pragma once

#include <libsettings/Watcher.h>

namespace Settings
{

class Setting
{
private:
    __noncopyable(Setting);
    __nonmovable(Setting);

    RefPtr<Service> _service;

    OwnPtr<Watcher> _watcher;
    Settings::Path _path;
    WatcherCallback _callback;
    Optional<Json::Value> _value;

public:
    Setting(const char *path, WatcherCallback callback)
    {
        _service = Service::the();

        _path = Path::parse(path);
        _callback = callback;
        _callback(read());
        _watcher = own<Watcher>(_path, [this](const auto &value) {
            _value = value;
            _callback(value);
        });
    }

    ~Setting()
    {
    }

    void write(const Json::Value &value)
    {
        _service->write(_path, value);
    }

    Json::Value read()
    {
        if (!_value)
        {
            _value = _service->read(_path);
        }

        return *_value;
    }
};

} // namespace Settings