#pragma once

#include <libsettings/Path.h>
#include <libsettings/storage/Domain.h>

namespace settings
{

struct Repository
{
    HashMap<String, Domain> domains;

    void write(const Path &path, json::Value value);

    json::Value read(const Path &path);
};

} // namespace settings
