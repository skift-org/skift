#include <libgraphic/Painter.h>
#include <libwidget/elements/SeparatorElement.h>

namespace Widget
{

void SeparatorElement::paint(Graphic::Painter &painter, const Math::Recti &dirty)
{
    painter.fill_rectangle(dirty, color(THEME_BORDER));
}

Math::Vec2i SeparatorElement::size()
{
    return Math::Vec2i(1, 1);
}

} // namespace Widget