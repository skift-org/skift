#pragma once

#include <libsettings/Bundle.h>

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
        if (!bundles.has_key(path.bundle))
        {
            return nullptr;
        }

        return bundles[path.bundle].read(path);
    }
};

} // namespace settings
