#include <libgraphic/Painter.h>
#include <libwidget/widgets/IconPanel.h>

void icon_panel_paint(IconPanel *widget, Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (!widget->icon)
    {
        return;
    }

    Rectangle destination = widget->icon->bound(ICON_18PX).centered_within(widget_get_content_bound(widget));

    painter.blit_icon(
        *widget->icon,
        ICON_18PX,
        destination,
        widget_get_color(widget, THEME_FOREGROUND));
}

Vec2i icon_panel_size(IconPanel *widget)
{
    if (widget->icon)
    {
        return widget->icon->bound(ICON_18PX).size();
    }
    else
    {
        return widget_get_bound(widget).size();
    }
}

static const WidgetClass icon_panel_class = {
    .paint = (WidgetPaintCallback)icon_panel_paint,
    .size = (WidgetComputeSizeCallback)icon_panel_size,
};

IconPanel *icon_panel_create(Widget *parent, RefPtr<Icon> icon)
{
    auto widget = __create(IconPanel);

    widget->icon = icon;
    widget_initialize(widget, &icon_panel_class, parent);

    return widget;
}
