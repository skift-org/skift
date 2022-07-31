#pragma once

#include <karm-math/rect.h>

#include "flow.h"

namespace Karm::Ui {

template <typename T>
struct Spacing {
    T start;
    T top;
    T end;
    T bottom;

    Spacing(T all = 0)
        : start(all), top(all), end(all), bottom(all) {}

    Spacing(T horizontal, T vertical)
        : start(horizontal), top(vertical), end(horizontal), bottom(vertical) {}

    Spacing(T start, T top, T end, T bottom)
        : start(start), top(top), end(end), bottom(bottom) {}

    Math::Rect<T> shrink(Flow flow, Math::Rect<T> rect) const {
        rect = flow.setStart(rect, flow.getStart(rect) + start);
        rect = flow.setTop(rect, flow.getTop(rect) + top);
        rect = flow.setEnd(rect, flow.getEnd(rect) - end);
        rect = flow.setBottom(rect, flow.getBottom(rect) - bottom);

        return rect;
    }

    Math::Rect<T> grow(Flow flow, Math::Rect<T> rect) const {
        rect = flow.setStart(rect, flow.getStart(rect) - start);
        rect = flow.setTop(rect, flow.getTop(rect) - top);
        rect = flow.setEnd(rect, flow.getEnd(rect) + end);
        rect = flow.setBottom(rect, flow.getBottom(rect) + bottom);

        return rect;
    }

    Math::Vec2<T> topStart() const {
        return {start, top};
    }

    Math::Vec2<T> topEnd() const {
        return {end, top};
    }

    Math::Vec2<T> bottomStart() const {
        return {start, bottom};
    }

    Math::Vec2<T> bottomEnd() const {
        return {end, bottom};
    }

    Math::Vec2<T> all() const {
        return {start + end, top + bottom};
    }
};

using Spacingi = Spacing<int>;
using Spacingf = Spacing<float>;

} // namespace Karm::Ui
