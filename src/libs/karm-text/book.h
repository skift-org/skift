#pragma once

#include <karm-mime/url.h>
#include <karm-sys/mmap.h>

#include "base.h"
#include "font.h"

namespace Karm::Text {

struct FontQuery {
    Family family = GenericFamily::SYSTEM;
    Opt<FontWeight> weight = FontWeight::REGULAR;
    Opt<FontStretch> stretch = FontStretch::NORMAL;
    Opt<FontStyle> style = FontStyle::NORMAL;
    Opt<Monospace> monospace = Monospace::NO;
};

struct FontInfo {
    Mime::Url url;
    FontAttrs attrs;
    Opt<Strong<Fontface>> face = NONE;
};

struct FontBook {
    FaceId _nextId = FaceId{1};
    Map<FaceId, FontInfo> _faces;
    Array<String, toUnderlyingType(GenericFamily::_LEN)> _genericFamily;

    FaceId add(FontInfo info) {
        auto id = _nextId;
        _nextId++;
        _faces.put(id, info);
        return id;
    }

    FaceId load(Mime::Url url, Opt<FontAttrs> attrs = NONE);

    Res<Strong<Fontface>> get(FaceId id);

    FaceId find(FontQuery query) {
        for (auto &[id, info] : _faces.iter()) {
            bool match = true;

            if (query.weight and info.attrs.weight != query.weight)
                match = false;

            if (query.stretch and info.attrs.stretch != query.stretch)
                match = false;

            if (query.style and info.attrs.style != query.style)
                match = false;

            if (query.monospace and info.attrs.monospace != query.monospace)
                match = false;

            if (match)
                return id;
        }

        return FaceId{0};
    }
};

Res<FontBook> loadAll();

} // namespace Karm::Text
