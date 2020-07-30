#include <libgraphic/Painter.h>
#include <libwidget/Window.h>

#include "file-explorer/Breadcrumb.h"

void breadcrumb_paint(Breadcrumb *widget, Painter &painter, Rectangle rectangle)
{
    __unused(widget);
    __unused(rectangle);

    int current = 0;

    Rectangle computer_icon_bound(
        widget_get_bound(widget).x(),
        widget_get_bound(widget).y(),
        widget->icon_computer->bound(ICON_18PX).width() + 16,
        widget_get_bound(widget).height());

    painter.blit_icon(
        *widget->icon_computer,
        ICON_18PX,
        widget->icon_computer->bound(ICON_18PX).centered_within(computer_icon_bound),
        widget_get_color(widget, THEME_FOREGROUND));

    current += computer_icon_bound.width();

    if (path_element_count(widget->path) != 0)
    {
        Rectangle expand_icon_bound(
            widget_get_bound(widget).x() + current,
            widget_get_bound(widget).y(),
            widget->icon_expand->bound(ICON_18PX).width(),
            widget_get_bound(widget).height());

        painter.blit_icon(
            *widget->icon_expand,
            ICON_18PX,
            widget->icon_expand->bound(ICON_18PX).centered_within(expand_icon_bound),
            widget_get_color(widget, THEME_FOREGROUND));

        current += expand_icon_bound.width();
    }

    for (size_t i = 0; i < path_element_count(widget->path); i++)
    {
        const char *element = path_peek_at(widget->path, i);

        int text_width = widget_font()->mesure_string(element).width();

        Rectangle element_bound(
            widget_get_bound(widget).x() + current,
            widget_get_bound(widget).y(),
            text_width,
            widget_get_bound(widget).height());

        painter.draw_string(
            *widget_font(),
            element,
            element_bound.position() + Vec2i(0, 19),
            widget_get_color(widget, THEME_FOREGROUND));

        current += text_width;

        if (i != path_element_count(widget->path) - 1)
        {
            Rectangle expand_icon_bound(
                widget_get_bound(widget).x() + current,
                widget_get_bound(widget).y(),
                widget->icon_expand->bound(ICON_18PX).width(),
                widget_get_bound(widget).height());

            painter.blit_icon(
                *widget->icon_expand,
                ICON_18PX,
                widget->icon_expand->bound(ICON_18PX).centered_within(expand_icon_bound),
                widget_get_color(widget, THEME_FOREGROUND));

            current += expand_icon_bound.width();
        }
    }
}

void breadcrumb_destroy(Breadcrumb *widget)
{
    widget->icon_computer = nullptr;
    widget->icon_expand = nullptr;

    path_destroy(widget->path);
}

static const WidgetClass breadcrumb_class = {
    .name = "Breadcrumb",

    .destroy = (WidgetDestroyCallback)breadcrumb_destroy,
    .paint = (WidgetPaintCallback)breadcrumb_paint,
};

Widget *breadcrumb_create(Widget *parent, const char *current_path)
{
    Breadcrumb *widget = __create(Breadcrumb);

    widget->path = path_create(current_path);
    widget->icon_computer = Icon::get("laptop");
    widget->icon_expand = Icon::get("chevron-right");

    widget_initialize(WIDGET(widget), &breadcrumb_class, parent);

    return WIDGET(widget);
}

void breadcrumb_navigate(Widget *widget, Path *path)
{
    Breadcrumb *breadcrumb = (Breadcrumb *)widget;

    path_destroy(breadcrumb->path);
    breadcrumb->path = path_clone(path);

    widget_update(widget);
}
