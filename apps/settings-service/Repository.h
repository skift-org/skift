#pragma once

#include <libsettings/Path.h>
#include <libsystem/io/Directory.h>
#include <libutils/Path.h>

#include "settings-service/Domain.h"

namespace settings
{

struct Repository
{
    HashMap<String, Domain> domains;

    static Repository load()
    {
        Repository repo;
        Directory *dir_repository = directory_open("/Configs", OPEN_READ);

        DirectoryEntry entry_repository;
        while (directory_read(dir_repository, &entry_repository))
        {
            if (entry_repository.stat.type != FILE_TYPE_DIRECTORY)
            {
                continue;
            }

            char buffer_domain[256];
            snprintf(buffer_domain, 256, "/Configs/%s", entry_repository.name);

            Domain domain;

            Directory *dir_domaine = directory_open(buffer_domain, OPEN_READ);

            DirectoryEntry entry_domaine;
            while (directory_read(dir_domaine, &entry_domaine))
            {
                if (entry_domaine.stat.type != FILE_TYPE_REGULAR)
                {
                    continue;
                }

                Bundle bundle{};

                char buffer_bundle[256];
                snprintf(buffer_bundle, 256, "/Configs/%s/%s", entry_repository.name, entry_domaine.name);

                auto value = json::parse_file(buffer_bundle);

                if (!value.is(json::OBJECT))
                {
                    continue;
                }

                auto obj = value.as_object();

                obj.foreach ([&](auto &key, auto &value) {
                    bundle.keys[key] = value;
                    return Iteration::CONTINUE;
                });

                auto path = ::Path::parse(entry_domaine.name);

                domain.bundles[path.basename_without_extension()] = move(bundle);
            }

            repo.domains[entry_repository.name] = move(domain);
        }

        directory_close(dir_repository);

        return repo;
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
            json::Object obj;

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

} // namespace settings
