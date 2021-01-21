#pragma once

#include <libsettings/Domain.h>
#include <libsettings/Path.h>

namespace settings
{

struct Repository
{
    HashMap<String, Domain> domains;

    Callback<void(const Path &path, const json::Value &value)> on_key_change;

    void write(const Path &path, const json::Value &value)
    {
        domains[path.domain].write(path, value);

        on_key_change(path, value);
    }

    json::Value read(const Path &path)
    {
        if (!domains.has_key(path.domain))
        {
            return json::NIL;
        }

        return domains[path.domain].read(path);
    }
};

} // namespace settings
