#pragma once

#include <karm-math/rect.h>
#include <mdi>

#include "canvas.h"

namespace Karm::Gfx {

struct Icon {
    Mdi::Icon _icon;
    f64 _size;

    Icon(Mdi::Icon icon, f64 size = 18)
        : _icon(icon), _size(size) {}

    Math::Rectf bound() {
        return {_size, _size};
    }

    Str name() const {
        return Str::fromNullterminated(_icon.name);
    }

    Str svg() const {
        return Str::fromNullterminated(_icon.path);
    }

    void fill(Gfx::Canvas &g, Math::Vec2i pos) const;

    void stroke(Gfx::Canvas &g, Math::Vec2i pos) const;
};

} // namespace Karm::Gfx
