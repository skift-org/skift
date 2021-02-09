#pragma once

#include "settings-service/Bundle.h"

namespace Settings
{

struct Domain
{
    HashMap<String, Bundle> bundles;

    static Domain load(const String &path)
    {
        Domain domain;
        System::Directory directory{path};

        for (auto entry : directory.entries())
        {
            if (entry.stat.type != FILE_TYPE_REGULAR)
            {
                continue;
            }

            auto bundle_path = String::format("{}/{}", path, entry.name);

            auto bundle = Bundle::Load(bundle_path);

            if (bundle)
            {
                auto path = ::Path::parse(bundle_path);

                domain.bundles[path.basename_without_extension()] = *bundle;
            }
        }

        return domain;
    }

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
            json::Value::Object obj;

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

} // namespace Settings
