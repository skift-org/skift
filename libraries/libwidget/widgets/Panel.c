#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libwidget/core/Theme.h>
#include <libwidget/widgets/Panel.h>

void panel_paint(Panel *panel, Painter *painter)
{
    painter_clear_rectangle(painter, WIDGET(panel)->bound, COLOR_BLUE);
    painter_draw_rectangle(painter, WIDGET(panel)->bound, COLOR_RED);
}

Widget *panel_create(Widget *parent, Rectangle bound)
{
    Panel *panel = __create(Panel);

    widget_initialize(WIDGET(panel), "Panel", parent, bound);

    WIDGET(panel)->destroy = NULL;
    WIDGET(panel)->paint = (WidgetPaintCallback)panel_paint;

    return WIDGET(panel);
}
