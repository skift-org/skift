#include <libgraphic/Painter.h>
#include <libwidget/Panel.h>

namespace Widget
{

Panel::Panel(Component *parent)
    : Component(parent)
{
}

void Panel::paint(Graphic::Painter &painter, const Math::Recti &)
{
    if (_border_radius > 0)
    {
        painter.fill_rectangle_rounded(bound(), _border_radius, color(THEME_MIDDLEGROUND));
    }
    else
    {
        painter.clear(bound(), color(THEME_MIDDLEGROUND));
    }
}

} // namespace Widget
