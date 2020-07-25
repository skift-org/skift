#include <libgraphic/Painter.h>
#include <libwidget/widgets/Separator.h>

void separator_paint(Separator *widget, Painter *painter, Rectangle rectangle)
{
    painter_fill_rectangle(painter, rectangle, widget_get_color(widget, THEME_BORDER));
}

Vec2i separator_size(Separator *widget)
{
    __unused(widget);

    return Vec2i(1, 1);
}

static const WidgetClass separator_class = {
    .name = "Separator",

    .paint = (WidgetPaintCallback)separator_paint,
    .size = (WidgetComputeSizeCallback)separator_size,
};

Widget *separator_create(Widget *parent)
{
    Separator *separator = __create(Separator);

    widget_initialize(WIDGET(separator), &separator_class, parent);

    return WIDGET(separator);
}
