#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libwidget/Panel.h>

void panel_paint(Panel *widget, Painter *painter, Rectangle rectangle)
{
    painter_clear_rectangle(painter, rectangle, widget_get_color(widget, THEME_MIDDLEGROUND));
}

Widget *panel_create(Widget *parent)
{
    Panel *panel = __create(Panel);

    WIDGET(panel)->paint = (WidgetPaintCallback)panel_paint;

    widget_initialize(WIDGET(panel), "Panel", parent);

    return WIDGET(panel);
}
