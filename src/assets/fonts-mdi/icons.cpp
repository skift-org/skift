#include "icons.h"

namespace Mdi {

char const *_names[] = {
#define ICON(id, name, code) name,
#include "defs/icons.inc"
#undef ICON
};

Rune _codepoints[] = {
#define ICON(id, name, code) code,
#include "defs/icons.inc"
#undef ICON
};

Slice<Rune> codepoints() {
    return Slice{
        _codepoints,
        sizeof(_codepoints) / sizeof(_codepoints[0]),
    };
}

Res<Icon> byName(Str query) {
    for (usize i = 0; i < sizeof(_names) / sizeof(_names[0]); ++i) {
        if (Str{_names[i]} == query)
            return Res<Icon>{Icon(_codepoints[i])};
    }
    return Error::notFound("icon not found");
}

Str name(Icon icon) {
    for (usize i = 0; i < sizeof(_codepoints) / sizeof(_codepoints[0]); ++i) {
        if (_codepoints[i] == (Rune)icon)
            return Str{_names[i]};
    }
    return "unknown";
}

} // namespace Mdi
