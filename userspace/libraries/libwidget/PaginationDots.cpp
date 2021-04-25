#include <libwidget/PaginationDots.h>

#include <libgraphic/Painter.h>

namespace Widget
{

PaginationDots::PaginationDots(int count)
    : _count(count)
{
    max_height(DOTSIZE);
    max_width(size().x());
}

PaginationDots::~PaginationDots()
{
}

void PaginationDots::paint(Graphic::Painter &painter, const Math::Recti &)
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

Math::Vec2i PaginationDots::size()
{
    return {DOTSIZE * _count + DOTSPACING * (_count - 1), DOTSIZE};
}

} // namespace Widget
