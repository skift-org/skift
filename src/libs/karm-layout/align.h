#pragma once

#include "flow.h"

namespace Karm::Layout {

struct Align {
    static constexpr int NONE = (0);

    static constexpr int START = (1 << 0);
    static constexpr int TOP = (1 << 1);
    static constexpr int END = (1 << 2);
    static constexpr int BOTTOM = (1 << 3);
    static constexpr int VSTRETCH = (1 << 4);
    static constexpr int HSTRETCH = (1 << 5);
    static constexpr int VCENTER = (1 << 6);
    static constexpr int HCENTER = (1 << 7);
    static constexpr int COVER = (1 << 8);
    static constexpr int FIT = (1 << 9);

    static constexpr int STRETCH = (HSTRETCH | VSTRETCH);
    static constexpr int VFILL = (VSTRETCH | TOP);
    static constexpr int HFILL = (HSTRETCH | START);
    static constexpr int CENTER = (HCENTER | VCENTER);
    static constexpr int FILL = (VFILL | HFILL);

    int _value = 0;

    Align(int value = 0) : _value(value) {}

    template <typename T>
    Math::Rect<T> apply(Flow flow, Math::Rect<T> inner, Math::Rect<T> outer) {
        if (_value == NONE)
            return outer;

        if (_value & COVER)
            inner = inner.cover(outer);

        if (_value & FIT)
            inner = inner.fit(outer);

        if (_value & START)
            inner = flow.setX(inner, flow.getStart(outer));

        if (_value & TOP)
            inner = flow.setY(inner, flow.getTop(outer));

        if (_value & END)
            inner = flow.setX(inner, flow.getEnd(outer) - flow.getWidth(inner));

        if (_value & BOTTOM)
            inner = flow.setY(inner, flow.getBottom(outer) - flow.getHeight(inner));

        if (_value & HSTRETCH)
            inner = flow.setWidth(inner, flow.getWidth(outer));

        if (_value & VSTRETCH)
            inner = flow.setHeight(inner, flow.getHeight(outer));

        if (_value & HCENTER)
            inner = flow.setX(inner, flow.getHcenter(outer) - flow.getWidth(inner) / 2);

        if (_value & VCENTER)
            inner = flow.setY(inner, flow.getVcenter(outer) - flow.getHeight(inner) / 2);

        return inner;
    }

    template <typename T>
    Math::Vec2<T> size(Math::Vec2<T> inner, Math::Vec2<T> outer) {
        if (_value & COVER)
            inner = outer;

        if (_value & HSTRETCH ||
            _value & HCENTER)
            inner.x = outer.x;

        if (_value & VSTRETCH ||
            _value & VCENTER)
            inner.y = outer.y;

        return inner;
    }
};

} // namespace Karm::Layout
