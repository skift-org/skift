#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libwidget/widgets/Panel.h>

void panel_paint(Panel *widget, Painter &painter, Rectangle rectangle)
{
    painter.clear_rectangle(rectangle, widget_get_color(widget, THEME_MIDDLEGROUND));
}

static const WidgetClass panel_class = {
    .name = "Panel",

    .paint = (WidgetPaintCallback)panel_paint,
};

Widget *panel_create(Widget *parent)
{
    Panel *panel = __create(Panel);

    widget_initialize(panel, &panel_class, parent);

    return panel;
}
