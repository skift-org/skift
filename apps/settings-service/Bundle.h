#pragma once

#include <libutils/json/Json.h>

#include <libsettings/Path.h>

namespace Settings
{

struct Bundle
{
    HashMap<String, json::Value> keys;

    static Optional<Bundle> Load(const String &path)
    {
        Bundle bundle;

        auto value = json::parse_file(path);

        if (!value.is(json::OBJECT))
        {
            return {};
        }

        auto obj = value.as_object();

        obj.foreach ([&](auto &key, auto &value) {
            bundle.keys[key] = value;
            return Iteration::CONTINUE;
        });

        return bundle;
    }

    void write(const Path &path, const json::Value &value)
    {
        keys[path.key] = value;
    }

    json::Value read(const Path &path)
    {
        if (path.key == "*")
        {
            json::Value::Object obj;

            keys.foreach ([&](auto &key, auto &value) {
                obj[key] = value;

                return Iteration::CONTINUE;
            });

            return obj;
        }
        else if (keys.has_key(path.key))
        {
            return keys[path.key];
        }
        else
        {
            return nullptr;
        }
    }
};

} // namespace Settings
