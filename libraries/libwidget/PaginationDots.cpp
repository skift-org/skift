#include <libwidget/PaginationDots.h>

#include <libgraphic/Painter.h>

PaginationDots::PaginationDots(Widget *parent, int count)
    : Widget(parent),
      _count(count)
{
    max_height(DOTSIZE);
    max_width(size().x());
}

PaginationDots::~PaginationDots()
{
}

void PaginationDots::paint(Painter &painter, const Recti &)
{
    for (int i = 0; i < _count; i++)
    {
        auto dot = content().column(_count, i, DOTSPACING);

        if (_index == i)
        {
            painter.fill_rectangle_rounded(dot, DOTSIZE, color(THEME_FOREGROUND));
        }
        else
        {
            painter.fill_rectangle_rounded(dot, DOTSIZE, color(THEME_BORDER));
        }
    }
}

Vec2i PaginationDots::size()
{
    return {DOTSIZE * _count + DOTSPACING * (_count - 1), DOTSIZE};
}