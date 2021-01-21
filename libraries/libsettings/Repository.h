#pragma once

#include <libsettings/Domain.h>
#include <libsettings/Path.h>

namespace settings
{

struct Repository
{
    HashMap<String, Domain> domains;

    void write(const Path &path, const json::Value &value)
    {
        if (!domains.has_key(path.domain))
        {
            domains[path.domain] = {};
        }

        domains[path.domain].write(path, value);
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
