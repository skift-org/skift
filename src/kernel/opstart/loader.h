#pragma once

import Mdi;
import Karm.Ui;
import Karm.Core;
import Karm.Image;
import Karm.Sys;
import Karm.Ref;
import Karm.Gfx;

#include <karm-logger/logger.h>

using namespace Karm;

namespace Opstart {

struct Blob {
    Ref::Url url;
    Serde::Value props;

    static Res<Blob> fromJson(Serde::Value const& json) {
        Blob blob = {};

        if (json.isStr()) {
            blob.url = Ref::Url::parse(json.asStr());
            return Ok(blob);
        }

        if (json.isObject()) {
            if (auto it = json.get("url"); it and it.isStr()) {
                blob.url = Ref::Url::parse(it.asStr());
            } else {
                return Error::invalidInput("missing url");
            }

            blob.props = json.get("props");

            return Ok(blob);
        }

        return Error::invalidInput("expected object or string");
    }
};

struct Entry {
    Union<None, Gfx::Icon, Rc<Gfx::Surface>> icon = NONE;
    String name;
    Blob kernel;
    Vec<Blob> blobs;

    static Res<Entry> fromJson(Serde::Value const& json) {
        if (not json.isObject())
            return Error::invalidInput("expected object");

        Entry entry = {};

        if (auto it = json.get("icon"); it and it.isStr()) {
            auto icon = Image::load(Ref::Url::parse(it.asStr()));
            if (icon)
                entry.icon = icon.unwrap();
        }

        if (auto it = json.get("name"); it and it.isStr()) {
            entry.name = it.asStr();
        } else {
            return Error::invalidInput("missing name");
        }

        if (auto it = json.get("kernel"); it and (it.isStr() or it.isObject())) {
            entry.kernel = try$(Blob::fromJson(it));
        } else {
            return Error::invalidInput("missing kernel");
        }

        if (auto it = json.get("blobs"); it and it.isArray()) {
            for (auto const& e : it.asArray()) {
                auto blob = try$(Blob::fromJson(e));
                entry.blobs.pushBack(blob);
            }
        }

        return Ok(entry);
    }
};

struct Configs {
    Opt<String> title;
    Opt<String> subtitle;
    Vec<Entry> entries;

    static Res<Configs> fromJson(Serde::Value const& json) {
        if (not json.isObject()) {
            return Error::invalidInput("expected array");
        }

        Configs configs = {};

        if (auto it = json.get("title"); it and it.isStr())
            configs.title = it.asStr();

        if (auto it = json.get("subtitle"); it and it.isStr())
            configs.subtitle = it.asStr();

        if (auto it = json.get("entries"); it and it.isArray()) {
            for (auto const& e : it.asArray()) {
                auto entry = try$(Entry::fromJson(e));
                configs.entries.pushBack(entry);
            }
        }

        return Ok(configs);
    }
};

Async::Task<> showMenuAsync(Sys::Context& ctx, Configs const& c);

Res<> loadEntry(Entry const&);

} // namespace Opstart
