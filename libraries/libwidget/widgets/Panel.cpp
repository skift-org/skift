#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libwidget/widgets/Panel.h>

Panel::Panel(Widget *parent)
    : Widget(parent)
{
}

void Panel::paint(Painter &painter, Recti rectangle)
{
    if (_border_radius > 0)
    {
        painter.fill_rectangle_rounded(bound(), _border_radius, color(THEME_MIDDLEGROUND));
    }
    else
    {
        painter.clear(rectangle, color(THEME_MIDDLEGROUND));
    }
}
