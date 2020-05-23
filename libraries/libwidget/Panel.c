#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libwidget/Panel.h>

void panel_paint(Panel *widget, Painter *painter, Rectangle rectangle)
{
    painter_clear_rectangle(painter, rectangle, widget_get_color(widget, THEME_MIDDLEGROUND));
}

static const WidgetClass panel_class = {
    .name = "Panel",

    .paint = (WidgetPaintCallback)panel_paint,
};

Widget *panel_create(Widget *parent)
{
    Panel *panel = __create(Panel);

    widget_initialize(WIDGET(panel), &panel_class, parent);

    return WIDGET(panel);
}
