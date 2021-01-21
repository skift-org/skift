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
        if (!keys.has_key(path.key))
        {
            return nullptr;
        }

        return keys[path.key];
    }
};

} // namespace settings
