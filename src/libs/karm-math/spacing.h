#pragma once

#include "flow.h"
#include "rect.h"

namespace Karm::Math {

template <typename T>
struct Spacing {
    T start;
    T top;
    T end;
    T bottom;

    constexpr Spacing()
        : start(), top(), end(), bottom() {}

    constexpr Spacing(T all)
        : start(all), top(all), end(all), bottom(all) {}

    constexpr Spacing(T horizontal, T vertical)
        : start(horizontal), top(vertical), end(horizontal), bottom(vertical) {}

    constexpr Spacing(T start, T top, T end, T bottom)
        : start(start), top(top), end(end), bottom(bottom) {}

    constexpr Math::Rect<T> shrink(Flow flow, Math::Rect<T> rect) const {
        rect = flow.setStart(rect, flow.getStart(rect) + start);
        rect = flow.setTop(rect, flow.getTop(rect) + top);
        rect = flow.setEnd(rect, flow.getEnd(rect) - end);
        rect = flow.setBottom(rect, flow.getBottom(rect) - bottom);

        return rect;
    }

    constexpr Math::Rect<T> grow(Flow flow, Math::Rect<T> rect) const {
        rect = flow.setStart(rect, flow.getStart(rect) - start);
        rect = flow.setTop(rect, flow.getTop(rect) - top);
        rect = flow.setEnd(rect, flow.getEnd(rect) + end);
        rect = flow.setBottom(rect, flow.getBottom(rect) + bottom);

        return rect;
    }

    constexpr Math::Vec2<T> topStart() const {
        return {start, top};
    }

    constexpr Math::Vec2<T> topEnd() const {
        return {end, top};
    }

    constexpr Math::Vec2<T> bottomStart() const {
        return {start, bottom};
    }

    constexpr Math::Vec2<T> bottomEnd() const {
        return {end, bottom};
    }

    constexpr Math::Vec2<T> all() const {
        return {start + end, top + bottom};
    }
};

using Spacingi = Spacing<isize>;
using Spacingf = Spacing<f64>;

} // namespace Karm::Math

template <typename T>
ReflectableTemplate$(Karm::Math::Spacing<T>, start, top, end, bottom);
