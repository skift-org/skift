#include <libgraphic/Painter.h>
#include <libwidget/Separator.h>

namespace Widget
{

Separator::Separator()
{
}

void Separator::paint(Graphic::Painter &painter, const Math::Recti &dirty)
{
    painter.fill_rectangle(dirty, color(THEME_BORDER));
}

Math::Vec2i Separator::size()
{
    return Math::Vec2i(1, 1);
}

} // namespace Widget
