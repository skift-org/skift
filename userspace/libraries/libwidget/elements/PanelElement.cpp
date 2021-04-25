#include <libgraphic/Painter.h>
#include <libwidget/elements/PanelElement.h>

namespace Widget
{

PanelElement::PanelElement(int radius)
{
    _border_radius = radius;
}

void PanelElement::paint(Graphic::Painter &painter, const Math::Recti &)
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
