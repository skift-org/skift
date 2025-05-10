#pragma once

#include "edge.h"
#include "trans.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
struct Poly {
    Vec<Edge<T>> _edges{};
    Opt<Rect<T>> _bound;

    Poly() = default;

    Rect<T> bound() {
        if (len() == 0)
            return {};

        if (_bound)
            return *_bound;

        Rect<T> res = _edges[0].bound();
        for (auto const& edge : *this)
            res = res.mergeWith(edge.bound());
        _bound = res;
        return *_bound;
    }

    Edgef const& operator[](usize i) const {
        return _edges[i];
    }

    Edgef const* buf() const {
        return _edges.buf();
    }

    usize len() const {
        return _edges.len();
    }

    Edge<T> const* begin() const {
        return buf();
    }

    Edge<T> const* end() const {
        return buf() + len();
    }

    void pushBack(Edge<T> edge) {
        if (edge.hasNan()) [[unlikely]]
            panic("nan in edge");
        _bound = NONE;
        _edges.pushBack(edge);
    }

    void offset(Vec2<T> off) {
        _bound = NONE;
        for (auto& e : _edges) {
            e = e + off;
        }
    }

    void clear() {
        _edges.clear();
        _bound = NONE;
    }

    void repr(Io::Emit& e) const {
        e("(poly {})", _edges);
    }

    void transform(Trans2<T> const& trans) {
        _bound = NONE;
        for (auto& e : _edges) {
            e.start = trans.apply(e.start);
            e.end = trans.apply(e.end);
        }
    }
};

using Polyf = Poly<f64>;
using Polyi = Poly<isize>;
using Polyu = Poly<usize>;

} // namespace Karm::Math
