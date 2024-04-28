#pragma once

#include "edge.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
struct Shape {
    Vec<Edge<T>> _edges{};

    Shape() = default;

    Rect<T> bound() const {
        Opt<Rect<T>> bound{};
        for (auto const &edge : *this) {
            if (bound) {
                bound = bound->mergeWith(edge.bound());
            } else {
                bound = edge.bound();
            }
        }
        return tryOr(bound, {});
    }

    Edgef const &operator[](usize i) const {
        return _edges[i];
    }

    Edgef const *buf() const {
        return _edges.buf();
    }

    usize len() const {
        return _edges.len();
    }

    Edge<T> const *begin() const {
        return buf();
    }

    Edge<T> const *end() const {
        return buf() + len();
    }

    void add(Edge<T> edge) {
        if (edge.hasNan()) [[unlikely]]
            panic("nan in edge");
        _edges.add(edge);
    }

    void offset(Vec2<T> off) {
        for (auto &e : _edges) {
            e = e + off;
        }
    }

    void clear() {
        _edges.clear();
    }
};

using Shapef = Shape<f64>;
using Shapei = Shape<isize>;
using Shapeu = Shape<usize>;

} // namespace Karm::Math
