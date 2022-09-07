#pragma once

#include "font.h"
#include "loader.h"

namespace Karm::Media {

enum struct Icons {
#define ICON(id, name, code) id = code,
#include "icons.inc"
#undef ICON
};

struct Icon {
    Icons _code;
    double _size;

    static Strong<Fontface> fontface();

    static Result<Icon> byName(Str query, double size = 18);

    Icon(Icons code, double size = 18)
        : _code(code), _size(size) {}

    Str name();

    double size() {
        return _size;
    }

    Math::Rectf bound() {
        return {_size, _size};
    }

    Icons code() {
        return _code;
    }

    void fill(Gfx::Context &g, Math::Vec2i pos) const;

    void stroke(Gfx::Context &g, Math::Vec2i pos) const;
};

} // namespace Karm::Media
