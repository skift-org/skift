#pragma once

#include <karm-base/set.h>
#include <karm-mime/url.h>
#include <karm-sys/mmap.h>

#include "base.h"
#include "font.h"

namespace Karm::Text {

struct FontQuery {
    Family family = GenericFamily::SYSTEM;
    FontWeight weight = FontWeight::REGULAR;
    FontStretch stretch = FontStretch::NORMAL;
    FontStyle style = FontStyle::NORMAL;
};

struct FontInfo {
    Mime::Url url;
    FontAttrs attrs;
    Strong<Fontface> face;
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

    Res<> loadAll();

    Res<FontInfo> get(FaceId id) const {
        auto res = _faces.tryGet(id);
        if (not res)
            return Error::notFound("font not found");
        return Ok(res.unwrap());
    }

    Set<String> families() const {
        Set<String> families;
        for (auto &[id, info] : _faces.iter())
            families.put(info.attrs.family);
        return families;
    }

    Str _resolveFamily(Family family) const {
        if (auto *gf = family.is<GenericFamily>()) {
            return _genericFamily[toUnderlyingType(*gf)];
        }
        return family.unwrap<Str>();
    }

    FaceId queryExact(FontQuery query) const;

    FaceId queryClosest(FontQuery query) const;

    Vec<FaceId> queryFamily(String family) const;
};

} // namespace Karm::Text
