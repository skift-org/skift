#pragma once

#include <libio/Format.h>

#include "settings-service/Bundle.h"

namespace Settings
{

struct Domain
{
    HashMap<String, Bundle> bundles;

    static Domain load(const String &path)
    {
        Domain domain;
        IO::Directory directory{path};

        for (auto entry : directory.entries())
        {
            if (entry.stat.type != HJ_FILE_TYPE_REGULAR)
            {
                continue;
            }

            auto bundle_path = IO::format("{}/{}", path, entry.name);

            auto bundle = Bundle::Load(bundle_path);

            if (bundle.present())
            {
                auto path = ::IO::Path::parse(bundle_path);

                domain.bundles[path.basename_without_extension()] = bundle.unwrap();
            }
        }

        return domain;
    }

    void write(const Settings::Path &path, const Json::Value &value)
    {
        if (!bundles.has_key(path.bundle))
        {
            bundles[path.bundle] = {};
        }

        bundles[path.bundle].write(path, value);
    }

    Json::Value read(const Settings::Path &path)
    {
        if (path.domain == "*")
        {
            Json::Value::Object obj;

            bundles.foreach([&](auto &key, auto &value) {
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

} // namespace Settings
