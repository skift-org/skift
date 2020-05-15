#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libwidget/Separator.h>

void separator_paint(Separator *widget, Painter *painter, Rectangle rectangle)
{
    painter_fill_rectangle(painter, rectangle, widget_get_color(widget, THEME_BORDER));
}

Vec2i separtor_size(Separator *widget)
{
    __unused(widget);

    return vec2i(1, 1);
}

Widget *separator_create(Widget *parent)
{
    Separator *separator = __create(Separator);

    WIDGET(separator)->paint = (WidgetPaintCallback)separator_paint;
    WIDGET(separator)->size = (WidgetComputeSizeCallback)separtor_size;

    widget_initialize(WIDGET(separator), "Separator", parent);

    return WIDGET(separator);
}
