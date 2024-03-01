#pragma once

#include <karm-base/opt.h>
#include <karm-base/string.h>

namespace Web::Html {

extern "C" unsigned int const *_Web__Html__lookupEntity(char const *name, size_t len);

static inline Opt<Slice<Rune>> lookupEntity(Str name) {
    Rune const *runes = _Web__Html__lookupEntity(name.buf(), name.len());
    if (runes == nullptr)
        return NONE;
    usize len = 0;
    while (runes[len] != 0)
        len++;
    return Slice<Rune>{runes, runes + len};
}

} // namespace Web::Html
