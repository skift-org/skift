#include <libgraphic/Painter.h>
#include <libwidget/elements/DotsElement.h>

namespace Widget
{

DotsElement::DotsElement(int count, int index)
    : _count{count}, _index{index}
{
    max_height(DOTSIZE);
    max_width(size().x());
}

void DotsElement::paint(Graphic::Painter &painter, const Math::Recti &)
{
    for (int i = 0; i < _count; i++)
    {
        auto dot = bound().column(_count, i, DOTSPACING);

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

Math::Vec2i DotsElement::size()
{
    return {DOTSIZE * _count + DOTSPACING * (_count - 1), DOTSIZE};
}

} // namespace Widget
