#include <libgraphic/Painter.h>
#include <libwidget/Separator.h>

namespace Widget
{

Separator::Separator(Component *parent) : Component(parent)
{
}

void Separator::paint(Graphic::Painter &painter, const Recti &dirty)
{
    painter.fill_rectangle(dirty, color(THEME_BORDER));
}

Vec2i Separator::size()
{
    return Vec2i(1, 1);
}

} // namespace Widget
