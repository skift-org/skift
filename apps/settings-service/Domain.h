#pragma once

#include "settings-service/Bundle.h"

namespace settings
{

struct Domain
{
    HashMap<String, Bundle> bundles;

    void write(const Path &path, const json::Value &value)
    {
        if (!bundles.has_key(path.bundle))
        {
            bundles[path.bundle] = {};
        }

        bundles[path.bundle].write(path, value);
    }

    json::Value read(const Path &path)
    {
        if (path.domain == "*")
        {
            json::Object obj;

            bundles.foreach ([&](auto &key, auto &value) {
                obj[key] = value.read(path);

                return Iteration::CONTINUE;
            });

            return obj;
        }
        else if (bundles.has_key(path.bundle))
        {
            return bundles[path.bundle].read(path);
        }
        else
        {
            return nullptr;
        }
    }
};

} // namespace settings
