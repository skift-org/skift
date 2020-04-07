#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libwidget/Panel.h>
#include <libwidget/Theme.h>

void panel_paint(Panel *panel, Painter *painter)
{
    painter_clear_rectangle(painter, WIDGET(panel)->bound, THEME_BACKGROUND);
    painter_draw_rectangle(painter, WIDGET(panel)->bound, THEME_ALT_BACKGROUND);
}

Widget *panel_create(Widget *parent)
{
    Panel *panel = __create(Panel);

    WIDGET(panel)->paint = (WidgetPaintCallback)panel_paint;

    widget_initialize(WIDGET(panel), "Panel", parent);

    return WIDGET(panel);
}
