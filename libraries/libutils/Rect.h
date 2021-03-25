#pragma once

#include <math.h>

#include <libutils/Anchor.h>
#include <libutils/Enum.h>
#include <libutils/Insets.h>
#include <libutils/Vec2.h>

#include <libsystem/math/MinMax.h>

enum Border : int
{
    NONE = 0,
    TOP = (1 << 0),
    BOTTOM = (1 << 1),
    LEFT = (1 << 2),
    RIGHT = (1 << 3),
};

template <typename Scalar>
struct __packed Rect
{
private:
    Scalar _x;
    Scalar _y;
    Scalar _width;
    Scalar _height;

public:
    static Rect empty()
    {
        return Rect(0, 0, 0, 0);
    }

    static Rect from_two_point(Vec2<Scalar> a, Vec2<Scalar> b)
    {
        auto x0 = MIN(a.x(), b.x());
        auto y0 = MIN(a.y(), b.y());
        auto x1 = MAX(a.x(), b.x());
        auto y1 = MAX(a.y(), b.y());

        return {x0, y0, x1 - x0, y1 - y0};
    }

    auto x() const { return _x; }

    auto y() const { return _y; }

    auto width() const { return _width; }

    auto height() const { return _height; }

    Vec2<Scalar> position() const { return {_x, _y}; }

    Vec2<Scalar> size() const { return {_width, _height}; }

    Vec2<Scalar> center() const { return position() + size() / 2; }

    auto top_left() const { return position(); }

    auto bottom_left() const { return position() + size().extract_y(); }

    auto top_right() const { return position() + size().extract_x(); }

    auto bottom_right() const { return position() + size(); }

    auto top() const { return y(); }

    auto bottom() const { return y() + height(); }

    auto left() const { return x(); }

    auto right() const { return x() + width(); }

    auto area() { return width() * height(); }

    bool is_empty() const { return _width == 0 || _height == 0; };

    Rect() = default;

    Rect(Scalar width, Scalar height)
        : _x(0),
          _y(0),
          _width(width),
          _height(height)
    {
    }

    Rect(Vec2<Scalar> size)
        : _x(0),
          _y(0),
          _width(size.x()),
          _height(size.y())
    {
    }

    Rect(Vec2<Scalar> position, Vec2<Scalar> size)
        : _x(position.x()),
          _y(position.y()),
          _width(size.x()),
          _height(size.y())
    {
    }

    Rect(int x, int y, int width, int height)
        : _x(x),
          _y(y),
          _width(width),
          _height(height)
    {
    }

    bool operator==(const Rect &other)
    {
        return _x == other._x &&
               _y == other._y &&
               _width == other._width &&
               _height == other._height;
    }

    bool operator!=(const Rect &other)
    {
        return !(*this == other);
    }

    Rect with_x(int x) const
    {
        Rect new_rect = Rect(*this);
        new_rect._x = x;
        return new_rect;
    }

    Rect with_y(int y) const
    {
        Rect new_rect = Rect(*this);
        new_rect._y = y;
        return new_rect;
    }

    Rect with_width(int width) const
    {
        Rect new_rect = Rect(*this);
        new_rect._width = width;
        return new_rect;
    }

    Rect with_height(int height) const
    {
        Rect new_rect = Rect(*this);
        new_rect._height = height;
        return new_rect;
    }

    Rect centered_within(Rect container) const
    {
        return Rect(
            container._x + container._width - container._width / 2 - _width / 2,
            container._y + container._height - container._height / 2 - _height / 2,
            _width,
            _height);
    }

    Rect place_within(Rect container, Anchor anchor) const
    {
        int x = container._x;
        int y = container._y;
        int width = _width;
        int height = _height;

        if (anchor == Anchor::CENTER ||
            anchor == Anchor::TOP_CENTER ||
            anchor == Anchor::BOTTOM_CENTER)
        {
            x += container._width / 2 - _width / 2;
        }

        if (anchor == Anchor::RIGHT ||
            anchor == Anchor::TOP_RIGHT ||
            anchor == Anchor::BOTTOM_RIGHT)
        {
            x += container._width - _width;
        }

        if (anchor == Anchor::LEFT ||
            anchor == Anchor::CENTER ||
            anchor == Anchor::RIGHT)
        {
            y += container._height / 2 - _height / 2;
        }

        if (anchor == Anchor::BOTTOM_LEFT ||
            anchor == Anchor::BOTTOM_CENTER ||
            anchor == Anchor::BOTTOM_RIGHT)
        {
            y += container._height - _height;
        }

        return Rect(x, y, width, height);
    }

    Rect merged_with(Rect other) const
    {
        Vec2<Scalar> topleft{
            MIN(_x, other._x),
            MIN(_y, other._y),
        };

        Vec2<Scalar> bottomright{
            MAX(_x + _width, other._x + other._width),
            MAX(_y + _height, other._y + other._height),
        };

        return Rect::from_two_point(topleft, bottomright);
    }

    Rect clipped_with(Rect other) const
    {
        if (!colide_with(other))
        {
            return empty();
        }

        Vec2<Scalar> topleft(
            MAX(left(), other.left()),
            MAX(top(), other.top()));

        Vec2<Scalar> bottomright(
            MIN(right(), other.right()),
            MIN(bottom(), other.bottom()));

        return Rect::from_two_point(topleft, bottomright);
    }

    Rect take_top(int size) const
    {
        return Rect(x(), y(), width(), size);
    }

    Rect take_top_left(int size) const
    {
        return take_top(size).take_left(size);
    }

    Rect take_top_right(int size) const
    {
        return take_top(size).take_right(size);
    }

    Rect take_bottom(int size) const
    {
        return Rect(x(), y() + height() - size, width(), size);
    }

    Rect take_bottom_left(int size) const
    {
        return take_bottom(size).take_left(size);
    }

    Rect take_bottom_right(int size) const
    {
        return take_bottom(size).take_right(size);
    }

    Rect take_left(int size) const
    {
        return Rect(x(), y(), size, height());
    }

    Rect take_right(int size) const
    {
        return Rect(x() + width() - size, y(), size, height());
    }

    Rect cutoff_top(int top) const
    {
        return cutoff_top_and_botton(top, 0);
    }

    Rect cutoff_botton(int bottom) const
    {
        return cutoff_top_and_botton(0, bottom);
    }

    Rect cutoff_top_and_botton(int top, int bottom) const
    {
        return Rect(x(), y() + top, width(), height() - top - bottom);
    }

    Rect cutoff_left(int left) const
    {
        return cutoff_left_and_right(left, 0);
    }

    Rect cutoff_right(int right) const
    {
        return cutoff_left_and_right(0, right);
    }

    Rect cutoff_left_and_right(int left, int right) const
    {
        return Rect(x() + left, y(), width() - left - right, height());
    }

    Rect row(int row_count, int index) const
    {
        return row(row_count, index, 0);
    }

    Rect row(int row_count, int index, int spacing) const
    {
        assert(index >= 0);
        assert(row_count > 0);

        int spacing_height = spacing * (row_count - 1);

        int available_height = height() - spacing_height;

        int row_height = available_height / row_count;

        int correction = available_height - (row_height * row_count);

        int current_row_height = row_height;

        if (index < correction)
        {
            current_row_height += 1;
        }

        int current_row_position =
            row_height * index +
            (spacing * index) +
            MIN(index, correction);

        return Rect{
            x(),
            y() + current_row_position,
            width(),
            row_height,
        };
    }

    Rect column(int column_count, int index) const
    {
        return column(column_count, index, 0);
    }

    Rect column(int column_count, int index, int spacing) const
    {
        assert(index >= 0);
        assert(column_count > 0);

        int spacing_width = spacing * (column_count - 1);

        int available_width = width() - spacing_width;

        int column_width = available_width / column_count;

        int correction = available_width - (column_width * column_count);

        int current_column_width = column_width;

        if (index < correction)
        {
            current_column_width += 1;
        }

        int current_column_position =
            column_width * index +
            (spacing * index) +
            MIN(index, correction);

        return Rect{
            x() + current_column_position,
            y(),
            current_column_width,
            height(),
        };
    }

    Rect shrinked(Insets<Scalar> insets) const
    {
        assert(insets.top() >= 0);
        assert(insets.bottom() >= 0);
        assert(insets.left() >= 0);
        assert(insets.right() >= 0);

        return Rect{
            _x + insets.left(),
            _y + insets.top(),
            MAX(0, _width - insets.left() - insets.right()),
            MAX(0, _height - insets.top() - insets.bottom()),
        };
    }

    Rect expended(Insets<Scalar> insets) const
    {
        assert(insets.top() >= 0);
        assert(insets.bottom() >= 0);
        assert(insets.left() >= 0);
        assert(insets.right() >= 0);

        return Rect(
            _x - insets.left(),
            _y - insets.top(),
            _width + insets.left() + insets.right(),
            _height + insets.top() + insets.bottom());
    }

    Rect resized(Vec2<Scalar> size) const
    {
        return Rect(
            _x,
            _y,
            size.x(),
            size.y());
    }

    Rect moved(Vec2<Scalar> position) const
    {
        return Rect(position, size());
    }

    Rect offset(Vec2<Scalar> offset) const
    {
        return Rect(position() + offset, size());
    }

    bool colide_with(Rect other) const
    {
        return _x < other._x + other._width &&
               _x + _width > other._x &&
               _y < other._y + other._height &&
               _height + _y > other._y;
    }

    bool contains(Vec2<Scalar> p) const
    {
        return left() <= p.x() && right() > p.x() &&
               top() <= p.y() && bottom() > p.y();
    }

    bool contains(Rect other) const
    {
        return left() <= other.left() && right() >= other.right() &&
               right() <= other.top() && bottom() >= other.bottom();
    }

    Border contains(Insets<Scalar> spacing, Vec2<Scalar> position) const
    {
        int borders = Border::NONE;

        if (take_top(spacing.top()).contains(position))
        {
            borders |= Border::TOP;
        }

        if (take_bottom(spacing.bottom()).contains(position))
        {
            borders |= Border::BOTTOM;
        }

        if (take_left(spacing.left()).contains(position))
        {
            borders |= Border::LEFT;
        }

        if (take_right(spacing.right()).contains(position))
        {
            borders |= Border::RIGHT;
        }

        return (Border)borders;
    }

    void substract(Rect other, Rect &t, Rect &b, Rect &l, Rect &r) const
    {
        if (colide_with(other))
        {
            other = other.clipped_with(*this);

            l = Rect(x(), y(), other.left() - left(), height());
            r = Rect(other.right(), y(), right() - other.right(), height());

            t = Rect(l.right(), y(), r.left() - l.right(), other.top() - top());
            b = Rect(l.right(), other.bottom(), r.left() - l.right(), bottom() - other.bottom());
        }
        else
        {
            t = *this;
            b = empty();
            l = empty();
            r = empty();
        }
    }

    Rect cover(Rect rect)
    {
        double scale_x = rect.width() / (double)width();
        double scale_y = rect.height() / (double)height();

        double scale = MAX(scale_x, scale_y);

        Rect scaled_rect{
            0,
            0,
            (Scalar)(width() * scale),
            (Scalar)(height() * scale)};

        return {rect.center() - scaled_rect.center(), scaled_rect.size()};
    }

    Rect fit(Rect rect)
    {
        double scale_x = rect.width() / (double)width();
        double scale_y = rect.height() / (double)height();

        double scale = MIN(scale_x, scale_y);

        Rect scaled_rect{
            0,
            0,
            (Scalar)(width() * scale),
            (Scalar)(height() * scale)};

        return {rect.center() - scaled_rect.center(), scaled_rect.size()};
    }

    Rect scaled(double scalex, double scaley)
    {
        return {
            x(),
            y(),
            (Scalar)(width() * scalex),
            (Scalar)(height() * scaley),
        };
    }
};

using Recti = Rect<int>;
using Rectf = Rect<float>;
using Rectd = Rect<double>;
