#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libwidget/widgets/RoundedPanel.h>

void rounded_panel_paint(RoundedPanel *widget, Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.fill_rounded_rectangle(widget_get_bound(widget), widget->radius, widget_get_color(widget, THEME_MIDDLEGROUND));
}

static const WidgetClass rounded_panel_class = {
    .name = "RoundedPanel",
    .paint = (WidgetPaintCallback)rounded_panel_paint,
};

RoundedPanel *rounded_panel_create(Widget *parent, int radius)
{
    auto rounded_panel = __create(RoundedPanel);

    rounded_panel->radius = radius;

    widget_initialize(rounded_panel, &rounded_panel_class, parent);

    return rounded_panel;
}
