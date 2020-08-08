#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libwidget/widgets/Panel.h>

void panel_paint(Panel *widget, Painter &painter, Rectangle rectangle)
{
    painter.clear_rectangle(rectangle, widget_get_color(widget, THEME_MIDDLEGROUND));
}

static const WidgetClass panel_class = {
    .paint = (WidgetPaintCallback)panel_paint,
};

Panel *panel_create(Widget *parent)
{
    auto panel = __create(Panel);

    widget_initialize(panel, &panel_class, parent);

    return panel;
}
