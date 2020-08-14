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
        painter.fill_rounded_rectangle(widget_get_bound(this), _border_radius, widget_get_color(this, THEME_MIDDLEGROUND));
    }
    else
    {
        painter.clear_rectangle(rectangle, widget_get_color(this, THEME_MIDDLEGROUND));
    }
}
