#pragma once

#include "path.h"
#include "style.h"

namespace Karm::Gfx {

struct Shape {
    Vec<Math::Edgef> _edges{};

    Shape() = default;

    Math::Rectf bound() const {
        Opt<Math::Rectf> bound{};
        for (auto const &edge : *this) {
            if (bound) {
                bound = bound->mergeWith(edge.bound());
            } else {
                bound = edge.bound();
            }
        }
        return tryOr(bound, {});
    }

    Math::Edgef const &operator[](usize i) const {
        return _edges[i];
    }

    Math::Edgef const *buf() const {
        return _edges.buf();
    }

    usize len() const {
        return _edges.len();
    }

    Math::Edgef const *begin() const {
        return buf();
    }

    Math::Edgef const *end() const {
        return buf() + len();
    }

    void add(Math::Edgef const &edge) {
        if (edge.hasNan()) {
            panic("NaN in edge");
        }
        _edges.add(edge);
    }

    void clear() {
        _edges.clear();
    }
};

void createStroke(Shape &shape, Path const &path, StrokeStyle stroke);

void createSolid(Shape &shape, Path &path);

} // namespace Karm::Gfx
