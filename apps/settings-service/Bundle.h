#pragma once

#include <libsystem/json/Json.h>

#include <libsettings/Path.h>

namespace settings
{

struct Bundle
{
    HashMap<String, json::Value> keys;

    void write(const Path &path, const json::Value &value)
    {
        keys[path.key] = value;
    }

    json::Value read(const Path &path)
    {
        if (path.key == "*")
        {
            json::Object obj;

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

} // namespace settings
