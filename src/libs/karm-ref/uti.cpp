export module Karm.Ref:uti;

import Karm.Core;

import :mime;

namespace Karm::Ref {

export struct Uti {
    Symbol _buf = ""_sym;

    enum struct Common {
#define UTI(NAME, ...) NAME,
#include "defs/uti.inc"

#undef UTI
    };

    using enum Common;

    Uti(Common c) {
        switch (c) {
#define UTI(NAME, STR, ...)           \
    case Common::NAME:                \
        _buf = Symbol::from(STR ""s); \
        break;
#include "defs/uti.inc"

#undef UTI
        }
    }

    Uti(Str str)
        : _buf{Symbol::from(str)} {}

    static Res<Uti> fromMime(Mime const& mime) {
#define UTI(NAME, STR, MIME, ...) \
    if (mime.is(Mime{MIME}))      \
        return Ok(Uti::NAME);
#include "defs/uti.inc"

#undef UTI

        return Error::invalidData("unknown mime type");
    }

    static Uti parse(Str str) {
        return Uti{str}; // lol
    }

    bool operator==(Uti const& other) const {
        return _buf == other._buf;
    }
};

} // namespace Karm::Ref

export auto operator""_uti(char const* str, Karm::usize len) {
    return Karm::Ref::Uti::parse({str, len});
}
