#include <libmath/Vec2.h>
#include <libutils/Vector.h>

#include <math.h>

template <typename TCallback>
void bresenhamLow(int x0, int y0, int x1, int y1, int size, TCallback callback)
{
    int dx, dy, i, p, x, y = 0;
    dx = x1 - x0;
    dy = y1 - y0;
    i = 1;
    if (dy < 0)
    {
        i = -1;
        dy = -dy;
    }
    x = x0;
    y = y0;

    p = 2 * dy - dx;

    while (x < x1)
    {
        callback(Math::Vec2(x, y) - Math::Vec2i(size / 2), Math::Vec2i(size, size));
        if (p >= 0)
        {
            y += i;
            p = p + 2 * dy - 2 * dx;
        }
        else
        {
            p = p + 2 * dy;
        }
        x++;
    }
}

template <typename TCallback>
void bresenhamHigh(int x0, int y0, int x1, int y1, int size, TCallback callback)
{
    int dx, dy, i, p, x, y = 0;
    dx = x1 - x0;
    dy = y1 - y0;
    i = 1;
    if (dx < 0)
    {
        i = -1;
        dx = -dx;
    }
    x = x0;
    y = y0;

    p = 2 * dx - dy;

    while (y < y1)
    {
        callback(Math::Vec2(x, y) - Math::Vec2i(size / 2), Math::Vec2i(size, size));
        if (p >= 0)
        {
            x += i;
            p = p + 2 * dx - 2 * dy;
        }
        else
        {
            p = p + 2 * dx;
        }
        y++;
    }
}

template <typename TCallback>
void bresenham(Math::Vec2i start, Math::Vec2i end, int size, TCallback callback)
{
    int x0 = start.x();
    int y0 = start.y();
    int x1 = end.x();
    int y1 = end.y();

    if (abs(y1 - y0) < abs(x1 - x0))
    {
        if (x0 > x1)
        {
            bresenhamLow(x1, y1, x0, y0, size, callback);
        }
        else
        {
            bresenhamLow(x0, y0, x1, y1, size, callback);
        }
    }
    else
    {
        if (y0 > y1)
        {
            bresenhamHigh(x1, y1, x0, y0, size, callback);
        }
        else
        {
            bresenhamHigh(x0, y0, x1, y1, size, callback);
        }
    }
}
