#pragma once

#include <karm-math/rect.h>
#include <karm-math/vec.h>

namespace Karm::Math {

enum struct Orien {
    NONE,
    HORIZONTAL,
    VERTICAL,
    BOTH
};

struct Flow {
    enum struct _Flow {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        TOP_TO_BOTTOM,
        BOTTOM_TO_TOP,
    };

    using enum _Flow;

    _Flow _flow{};

    constexpr Flow(_Flow flow = LEFT_TO_RIGHT)
        : _flow(flow) {}

    constexpr Orien orien() const {
        return (_flow == LEFT_TO_RIGHT or _flow == RIGHT_TO_LEFT)
                   ? Orien::HORIZONTAL
                   : Orien::VERTICAL;
    }

    constexpr Flow relative(Flow child) const {
        Flow table[4 * 4] = {
            LEFT_TO_RIGHT,
            RIGHT_TO_LEFT,
            TOP_TO_BOTTOM,
            BOTTOM_TO_TOP,

            RIGHT_TO_LEFT,
            LEFT_TO_RIGHT,
            TOP_TO_BOTTOM,
            BOTTOM_TO_TOP,

            TOP_TO_BOTTOM,
            BOTTOM_TO_TOP,
            LEFT_TO_RIGHT,
            RIGHT_TO_LEFT,

            BOTTOM_TO_TOP,
            TOP_TO_BOTTOM,
            LEFT_TO_RIGHT,
            RIGHT_TO_LEFT,
        };

        return table[(isize)_flow * 4 + (isize)child._flow];
    }

    template <typename T>
    Vec2<T> vec() const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
            return {1, 0};

        case RIGHT_TO_LEFT:
            return {-1, 0};

        case TOP_TO_BOTTOM:
            return {0, 1};

        case BOTTOM_TO_TOP:
            return {0, -1};
        }
    }

    template <typename T>
    T getStart(Rect<T> rect) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
            return rect.x;

        case RIGHT_TO_LEFT:
            return rect.x + rect.width;

        case TOP_TO_BOTTOM:
            return rect.y;

        case BOTTOM_TO_TOP:
            return rect.y + rect.height;
        }
    }

    template <typename T>
    T getX(Vec2<T> vec) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            return vec.x;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            return vec.y;
        }
    }

    template <typename T>
    T getEnd(Rect<T> rect) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
            return rect.x + rect.width;

        case RIGHT_TO_LEFT:
            return rect.x;

        case TOP_TO_BOTTOM:
            return rect.y + rect.height;

        case BOTTOM_TO_TOP:
            return rect.y;
        }
    }

    template <typename T>
    T getTop(Rect<T> rect) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            return rect.y;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            return rect.x;
        }
    }

    template <typename T>
    T getY(Vec2<T> vec) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            return vec.y;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            return vec.x;
        }
    }

    template <typename T>
    T getBottom(Rect<T> rect) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            return rect.y + rect.height;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            return rect.x + rect.width;
        }
    }

    template <typename T>
    T getWidth(Rect<T> rect) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            return rect.width;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            return rect.height;
        }
    }

    template <typename T>
    T getHeight(Rect<T> rect) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            return rect.height;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            return rect.width;
        }
    }

    template <typename T>
    Vec2<T> getOrigin(Rect<T> rect) const {
        return {getStart(rect), getTop(rect)};
    }

    template <typename T>
    T getHcenter(Rect<T> rect) const {
        return (getStart(rect) + getEnd(rect)) / 2;
    }

    template <typename T>
    T getVcenter(Rect<T> rect) const {
        return (getTop(rect) + getBottom(rect)) / 2;
    }

    template <typename T>
    Rect<T> setStart(Rect<T> rect, T value) const {
        T d = value - getStart(rect);

        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
            rect.x += d;
            rect.width -= d;
            return rect;

        case RIGHT_TO_LEFT:
            rect.width += d;
            return rect;

        case TOP_TO_BOTTOM:
            rect.y += d;
            rect.height -= d;
            return rect;

        case BOTTOM_TO_TOP:
            rect.height += d;
            return rect;
        }
    }

    template <typename T>
    Rect<T> setX(Rect<T> rect, T value) const {
        T d = value - getStart(rect);

        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            rect.x += d;
            return rect;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            rect.y += d;
            return rect;
        }
    }

    template <typename T>
    Rect<T> setEnd(Rect<T> rect, T value) const {
        T d = value - getEnd(rect);

        switch (_flow) {
        case RIGHT_TO_LEFT:
            rect.x += d;
            [[fallthrough]];

        default:
        case LEFT_TO_RIGHT:
            rect.width += d;
            return rect;

        case BOTTOM_TO_TOP:
            rect.y += d;
            [[fallthrough]];

        case TOP_TO_BOTTOM:
            rect.height += d;
            return rect;
        }
    }

    template <typename T>
    Rect<T> setTop(Rect<T> rect, T value) const {
        T d = value - getTop(rect);

        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            rect.y += d;
            rect.height -= d;
            return rect;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            rect.x += d;
            rect.width -= d;
            return rect;
        }
    }

    template <typename T>
    Rect<T> setY(Rect<T> rect, T value) const {
        T d = value - getTop(rect);

        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            rect.y += d;
            return rect;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            rect.x += d;
            return rect;
        }
    }

    template <typename T>
    Rect<T> setBottom(Rect<T> rect, T value) const {
        T d = value - getBottom(rect);

        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            rect.height += d;
            return rect;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            rect.width += d;
            return rect;
        }
    }

    template <typename T>
    Rect<T> setOrigin(Rect<T> rect, Vec2<T> value) const {
        rect = setX(rect, value.x);
        rect = setY(rect, value.y);
        return rect;
    }

    template <typename T>
    Rect<T> setWidth(Rect<T> rect, T value) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            rect.width = value;
            return rect;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            rect.height = value;
            return rect;
        }
    }

    template <typename T>
    Rect<T> setHeight(Rect<T> rect, T value) const {
        switch (_flow) {
        default:
        case LEFT_TO_RIGHT:
        case RIGHT_TO_LEFT:
            rect.height = value;
            return rect;

        case TOP_TO_BOTTOM:
        case BOTTOM_TO_TOP:
            rect.width = value;
            return rect;
        }
    }
};

} // namespace Karm::Math
