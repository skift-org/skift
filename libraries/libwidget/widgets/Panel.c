#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libwidget/core/Theme.h>
#include <libwidget/widgets/Panel.h>

void panel_paint(Panel *panel, Painter *painter)
{
    painter_clear_rectangle(painter, WIDGET(panel)->bound, THEME_BACKGROUND);
    painter_draw_rectangle(painter, WIDGET(panel)->bound, THEME_ALT_BACKGROUND);
}

Widget *panel_create(Widget *parent, Rectangle bound)
{
    Panel *panel = __create(Panel);

    WIDGET(panel)->destroy = NULL;
    WIDGET(panel)->paint = (WidgetPaintCallback)panel_paint;

    widget_initialize(WIDGET(panel), "Panel", parent, bound);

    return WIDGET(panel);
}
