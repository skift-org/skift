#include <libgraphic/Painter.h>
#include <libwidget/Panel.h>

Panel::Panel(Widget *parent)
    : Widget(parent)
{
}

void Panel::paint(Painter &painter, const Recti &)
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
