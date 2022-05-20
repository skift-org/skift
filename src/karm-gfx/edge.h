#pragma once

#include <karm-base/iter.h>
#include <karm-base/vec.h>
#include <karm-math/vec.h>

namespace Karm::Gfx {

struct Vert {
    Math::Vec2f pos;
    static constexpr auto END = 0x1;
    uint8_t flags;

    operator Math::Vec2f() const { return pos; }
};

struct Edge {
    Math::Vec2f start;
    Math::Vec2f end;
};

struct Subpath {
    Vert *begin;
    size_t len;
};

struct Path {
    Vec<Vert> _edges;

    void clear() {
        _edges.clear();
    }
};

} // namespace Karm::Gfx
