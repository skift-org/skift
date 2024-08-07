#pragma once

#include <karm-text/loader.h>
#include <mdi/spec.h>

namespace Karm::Media {

struct Icon {
    Mdi::Icon _code;
    f64 _size;

    static Strong<Text::Fontface> fontface();

    static Res<Icon> byName(Str query, f64 size = 18) {
        return Ok(Icon(try$(Mdi::byName(query)), size));
    }

    Icon(Mdi::Icon code, f64 size = 18)
        : _code(code), _size(size) {}

    Str name() {
        return Mdi::name(_code);
    }

    f64 size() {
        return _size;
    }

    Math::Rectf bound() {
        return {_size, _size};
    }

    Mdi::Icon code() {
        return _code;
    }

    void fill(Gfx::Canvas &g, Math::Vec2i pos) const;

    void stroke(Gfx::Canvas &g, Math::Vec2i pos) const;
};

} // namespace Karm::Media
