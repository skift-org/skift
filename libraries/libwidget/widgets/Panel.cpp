#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libwidget/widgets/Panel.h>

Panel::Panel(Widget *parent)
    : Widget(parent)
{
}

void Panel::paint(Painter &painter, Rectangle rectangle)
{
    if (_border_radius > 0)
    {
        painter.fill_rounded_rectangle(bound(), _border_radius, color(THEME_MIDDLEGROUND));
    }
    else
    {
        painter.clear_rectangle(rectangle, color(THEME_MIDDLEGROUND));
    }
}
