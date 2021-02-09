#pragma once

#include <libsettings/Path.h>
#include <libsystem/io_new/Directory.h>
#include <libutils/Path.h>
#include <stdio.h>

#include "settings-service/Domain.h"

namespace Settings
{

struct Repository
{
    HashMap<String, Domain> domains;

    static Repository load()
    {
        Repository repository;
        System::Directory dir_repository{"/Configs"};

        for (auto entry : dir_repository.entries())
        {
            if (entry.stat.type != FILE_TYPE_DIRECTORY)
            {
                continue;
            }

            auto domain_path = String::format("/Configs/{}", entry.name);
            System::Directory dir_domain{domain_path};

            auto domain = Domain::load(domain_path);

            repository.domains[entry.name] = domain;
        }

        return repository;
    }

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
        if (path.domain == "*")
        {
            json::Value::Object obj;

            domains.foreach ([&](auto &key, auto &value) {
                obj[key] = value.read(path);

                return Iteration::CONTINUE;
            });

            return obj;
        }
        else if (domains.has_key(path.domain))
        {
            return domains[path.domain].read(path);
        }
        else
        {
            return nullptr;
        }
    }
};

} // namespace Settings
