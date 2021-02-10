#pragma once

#include <libgraphic/Icon.h>
#include <libutils/json/Json.h>
#include <libutils/Path.h>
#include <libutils/RefPtr.h>

namespace filepicker
{

struct Bookmark
{
private:
    String _name;
    RefPtr<Icon> _icon;
    Path _path;

public:
    const String &name() const
    {
        return _name;
    }

    const RefPtr<Icon> icon() const
    {
        return _icon;
    }

    const Path &path() const
    {
        return _path;
    }

    Bookmark(const json::Value &value)
    {
        if (value.is(json::OBJECT))
        {
            value.with("name", [&](auto &value) {
                _name = value.as_string();
            });

            value.with("icon", [&](auto &value) {
                _icon = Icon::get(value.as_string());
            });

            value.with("path", [&](auto &value) {
                _path = Path::parse(value.as_string());
            });
        }
    }

    Bookmark(const String &name, const RefPtr<Icon> icon, const Path &path)
        : _name(name),
          _icon(icon),
          _path(path)
    {
    }

    bool operator==(const Bookmark &other) const
    {
        return _path == other._path;
    }

    json::Value serialize()
    {
        json::Value::Object obj{};

        obj["name"] = _name;
        obj["icon"] = _icon->name();
        obj["path"] = _path.string();

        return obj;
    }
};

} // namespace filepicker