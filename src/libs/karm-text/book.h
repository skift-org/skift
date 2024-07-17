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

    void repr(Io::Emit &e) const {
        e.ln("family: {#}", family);
        e.ln("weight: {}", weight);
        e.ln("stretch: {}", stretch);
        e.ln("style: {}", style);
    }
};

struct FontInfo {
    Mime::Url url;
    FontAttrs attrs;
    Strong<Fontface> face;
};

Str commonFamily(Str lhs, Str rhs);

struct FontBook {
    Vec<FontInfo> _faces;
    Array<String, toUnderlyingType(GenericFamily::_LEN)> _genericFamily;

    void add(FontInfo info) {
        _faces.pushBack(info);
    }

    Strong<Fontface> load(Mime::Url url, Opt<FontAttrs> attrs = NONE);

    Res<> loadAll();

    Vec<String> families() const;

    Str _resolveFamily(Family family) const;

    Opt<Strong<Fontface>> queryExact(FontQuery query) const;

    Opt<Strong<Fontface>> queryClosest(FontQuery query) const;

    Vec<Strong<Fontface>> queryFamily(String family) const;
};

} // namespace Karm::Text
