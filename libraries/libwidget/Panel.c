#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libwidget/Panel.h>
#include <libwidget/Theme.h>

void panel_paint(Panel *widget, Painter *painter, Rectangle rectangle)
{
    __unused(widget);
    __unused(rectangle);
    painter_clear_rectangle(painter, rectangle, THEME_ALT_BACKGROUND);
}

Widget *panel_create(Widget *parent)
{
    Panel *panel = __create(Panel);

    WIDGET(panel)->paint = (WidgetPaintCallback)panel_paint;

    widget_initialize(WIDGET(panel), "Panel", parent);

    return WIDGET(panel);
}
