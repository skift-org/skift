#pragma once

#include <libsystem/Logger.h>
#include <libsystem/math/Math.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/math/Vectors.h>
#include <libutils/Enum.h>

struct Insets
{
private:
    int _top;
    int _bottom;
    int _left;
    int _right;

public:
    int top() const { return _top; }
    int bottom() const { return _bottom; }
    int left() const { return _left; }
    int right() const { return _right; }

    Insets()
        : Insets(0, 0, 0, 0)
    {
    }

    Insets(int tblr)
        : Insets(tblr, tblr, tblr, tblr)
    {
    }

    Insets(int tb, int lr)
        : Insets(tb, tb, lr, lr)
    {
    }

    Insets(int top, int bottom, int lr)
        : Insets(top, bottom, lr, lr)
    {
    }

    Insets(int top, int bottom, int left, int right)
        : _top(top),
          _bottom(bottom),
          _left(left),
          _right(right)
    {
    }
};

enum class Position
{
    LEFT,
    CENTER,
    RIGHT,
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT,
};

enum RectangleBorder : uint32_t
{
    NONE = 0,
    TOP = (1 << 0),
    BOTTOM = (1 << 1),
    LEFT = (1 << 2),
    RIGHT = (1 << 3),
};
__enum_flags(RectangleBorder);

struct __packed Rectangle
{
private:
    int _x;
    int _y;
    int _width;
    int _height;

public:
    static Rectangle empty()
    {
        return Rectangle(0, 0, 0, 0);
    }

    static Rectangle from_two_point(Vec2i a, Vec2i b)
    {
        int x0 = MIN(a.x(), b.x());
        int y0 = MIN(a.y(), b.y());
        int x1 = MAX(a.x(), b.x());
        int y1 = MAX(a.y(), b.y());

        return {x0, y0, x1 - x0, y1 - y0};
    }

    int x() const { return _x; }

    int y() const { return _y; }

    int width() const { return _width; }

    int height() const { return _height; }

    Vec2i position() const { return Vec2i(_x, _y); }

    Vec2i size() const { return Vec2i(_width, _height); }

    Vec2i center() const { return position() + size() / 2; }

    Vec2i top_left() const
    {
        return position();
    }

    Vec2i bottom_left() const
    {
        return position() + size().extract_y();
    }

    Vec2i top_right() const
    {
        return position() + size().extract_x();
    }

    Vec2i bottom_right() const
    {
        return position() + size();
    }

    int top() const
    {
        return y();
    }

    int bottom() const
    {
        return y() + height();
    }

    int left() const
    {
        return x();
    }

    int right() const
    {
        return x() + width();
    }

    int area() { return width() * height(); }

    bool is_empty() const { return _width == 0 || _height == 0; };

    Rectangle() = default;

    Rectangle(int width, int height)
        : _x(0),
          _y(0),
          _width(width),
          _height(height)
    {
    }

    Rectangle(Vec2i size)
        : _x(0),
          _y(0),
          _width(size.x()),
          _height(size.y())
    {
    }

    Rectangle(Vec2i position, Vec2i size)
        : _x(position.x()),
          _y(position.y()),
          _width(size.x()),
          _height(size.y())
    {
    }

    Rectangle(int x, int y, int width, int height)
        : _x(x),
          _y(y),
          _width(width),
          _height(height)
    {
    }

    Rectangle with_width(int width) const
    {
        Rectangle new_rect = Rectangle(*this);
        new_rect._width = width;
        return new_rect;
    }

    Rectangle with_height(int height) const
    {
        Rectangle new_rect = Rectangle(*this);
        new_rect._height = height;
        return new_rect;
    }

    Rectangle centered_within(Rectangle container) const
    {
        return Rectangle(
            container._x + container._width - container._width / 2 - _width / 2,
            container._y + container._height - container._height / 2 - _height / 2,
            _width,
            _height);
    }

    Rectangle place_within(Rectangle container, Position position) const
    {
        int x = container._x;
        int y = container._y;
        int width = _width;
        int height = _height;

        if (position == Position::CENTER ||
            position == Position::TOP_CENTER ||
            position == Position::BOTTOM_CENTER)
        {
            x += container._width / 2 - _width / 2;
        }

        if (position == Position::RIGHT ||
            position == Position::TOP_RIGHT ||
            position == Position::BOTTOM_RIGHT)
        {
            x += container._width - _width;
        }

        if (position == Position::LEFT ||
            position == Position::CENTER ||
            position == Position::RIGHT)
        {
            y += container._height / 2 - _height / 2;
        }

        if (position == Position::BOTTOM_LEFT ||
            position == Position::BOTTOM_CENTER ||
            position == Position::BOTTOM_RIGHT)
        {
            y += container._height - _height;
        }

        return Rectangle(x, y, width, height);
    }

    Rectangle merged_with(Rectangle other) const
    {
        Vec2i topleft(
            MIN(_x, other._x),
            MIN(_y, other._y));

        Vec2i bottomright(
            MAX(_x + _width, other._x + other._width),
            MAX(_y + _height, other._y + other._height));

        return Rectangle::from_two_point(topleft, bottomright);
    }

    Rectangle clipped_with(Rectangle other) const
    {
        if (!colide_with(other))
        {
            return empty();
        }

        Vec2i topleft(
            MAX(left(), other.left()),
            MAX(top(), other.top()));

        Vec2i bottomright(
            MIN(right(), other.right()),
            MIN(bottom(), other.bottom()));

        return Rectangle::from_two_point(topleft, bottomright);
    }

    Rectangle take_top(int size) const
    {
        return Rectangle(x(), y(), width(), size);
    }

    Rectangle take_bottom(int size) const
    {
        return Rectangle(x(), y() + height() - size, width(), size);
    }

    Rectangle take_left(int size) const
    {
        return Rectangle(x(), y(), size, height());
    }

    Rectangle take_right(int size) const
    {
        return Rectangle(x() + width() - size, y(), size, height());
    }

    Rectangle cutoff_top(int top) const
    {
        return cutoff_top_and_botton(top, 0);
    }

    Rectangle cutoff_botton(int bottom) const
    {
        return cutoff_top_and_botton(0, bottom);
    }

    Rectangle cutoff_top_and_botton(int top, int bottom) const
    {
        return Rectangle(x(), y() + top, width(), height() - top - bottom);
    }

    Rectangle cutoff_left(int left) const
    {
        return cutoff_left_and_right(left, 0);
    }

    Rectangle cutoff_right(int right) const
    {
        return cutoff_left_and_right(0, right);
    }

    Rectangle cutoff_left_and_right(int left, int right) const
    {
        return Rectangle(x() + left, y(), width() - left - right, height());
    }

    Rectangle row(int row_count, int index) const
    {
        return row(row_count, index, 0);
    }

    Rectangle row(int row_count, int index, int spacing) const
    {
        assert(index >= 0);
        assert(row_count > 0);

        int spacing_height = spacing * row_count - 1;

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

        return Rectangle{
            x(),
            y() + current_row_position,
            width(),
            row_height,
        };
    }

    Rectangle column(int column_count, int index) const
    {
        return column(column_count, index, 0);
    }

    Rectangle column(int column_count, int index, int spacing) const
    {
        assert(index >= 0);
        assert(column_count > 0);

        int spacing_width = spacing * column_count - 1;

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

        return Rectangle{
            x() + current_column_position,
            y(),
            current_column_width,
            height(),
        };
    }

    Rectangle shrinked(Insets insets) const
    {
        assert(insets.top() >= 0);
        assert(insets.bottom() >= 0);
        assert(insets.left() >= 0);
        assert(insets.right() >= 0);

        return Rectangle(
            _x + insets.left(),
            _y + insets.top(),
            MAX(0, _width - insets.left() - insets.right()),
            MAX(0, _height - insets.top() - insets.bottom()));
    }

    Rectangle expended(Insets insets) const
    {
        assert(insets.top() >= 0);
        assert(insets.bottom() >= 0);
        assert(insets.left() >= 0);
        assert(insets.right() >= 0);

        return Rectangle(
            _x - insets.left(),
            _y - insets.top(),
            _width + insets.left() + insets.right(),
            _height + insets.top() + insets.bottom());
    }

    Rectangle resized(Vec2i size) const
    {
        return Rectangle(
            _x,
            _y,
            size.x(),
            size.y());
    }

    Rectangle moved(Vec2i position) const
    {
        return Rectangle(position, size());
    }

    Rectangle offset(Vec2i offset) const
    {
        return Rectangle(position() + offset, size());
    }

    bool colide_with(Rectangle other) const
    {
        return _x < other._x + other._width &&
               _x + _width > other._x &&
               _y < other._y + other._height &&
               _height + _y > other._y;
    }

    bool contains(Vec2i p) const
    {
        return p.x() >= left() && p.x() < right() &&
               p.y() >= top() && p.y() < bottom();
    }

    bool contains(Rectangle other) const
    {
        return (_x <= other._x && (_x + _width) >= (other._x + other._width)) &&
               (_y <= other._y && (_y + _height) >= (other._y + other._width));
    }

    RectangleBorder contains(Insets spacing, Vec2i position) const
    {
        RectangleBorder borders = RectangleBorder::NONE;

        if (take_top(spacing.top()).contains(position))
        {
            borders |= RectangleBorder::TOP;
        }

        if (take_bottom(spacing.bottom()).contains(position))
        {
            borders |= RectangleBorder::BOTTOM;
        }

        if (take_left(spacing.left()).contains(position))
        {
            borders |= RectangleBorder::LEFT;
        }

        if (take_right(spacing.right()).contains(position))
        {
            borders |= RectangleBorder::RIGHT;
        }

        return borders;
    }

    void substract(Rectangle other, Rectangle &t, Rectangle &b, Rectangle &l, Rectangle &r) const
    {
        if (colide_with(other))
        {
            other = other.clipped_with(*this);

            l = Rectangle(x(), y(), other.left() - left(), height());
            r = Rectangle(other.right(), y(), right() - other.right(), height());

            t = Rectangle(l.right(), y(), r.left() - l.right(), other.top() - top());
            b = Rectangle(l.right(), other.bottom(), r.left() - l.right(), bottom() - other.bottom());
        }
        else
        {
            t = *this;
            b = empty();
            l = empty();
            r = empty();
        }
    }
};
