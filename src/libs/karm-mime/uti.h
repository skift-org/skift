#pragma once

#include <karm-io/fmt.h>

namespace Karm::Mime {

struct Uti {
    String _buf;

    enum struct Common {
#define UTI(NAME, ...) NAME,
#include "defs/uti.inc"
#undef UTI
    };

    using enum Common;

    Uti(Common c) {
        switch (c) {
#define UTI(NAME, STR)  \
    case Common::NAME:  \
        _buf = STR ""s; \
        break;
#include "defs/uti.inc"
#undef UTI
        }
    }

    Uti(String str) : _buf{str} {}

    static Uti parse(Str str) {
        return Uti{str}; // lol
    }

    bool operator==(Uti const &other) const {
        return _buf == other._buf;
    }
};

} // namespace Karm::Mime

inline auto operator""_uti(char const *str, usize len) {
    return Karm::Mime::Uti::parse({str, len});
}
