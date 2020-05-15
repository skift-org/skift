#include <libgraphic/Painter.h>
#include <libwidget/IconCache.h>
#include <libwidget/Window.h>

#include "file-explorer/Breadcrumb.h"

void breadcrumb_paint(Breadcrumb *widget, Painter *painter, Rectangle rectangle)
{
    __unused(widget);
    __unused(rectangle);

    int current = 0;

    Rectangle computer_icon_bound = (Rectangle){{
        widget_bound(widget).x,
        widget_bound(widget).y,
        bitmap_bound(widget->icon_computer).width,
        widget_bound(widget).height,
    }};

    painter_blit_icon(
        painter,
        widget->icon_computer,
        rectangle_center_within(
            bitmap_bound(widget->icon_computer),
            computer_icon_bound),
        widget_get_color(widget, THEME_FOREGROUND));

    current += computer_icon_bound.width;

    if (path_element_count(widget->path) != 0)
    {
        Rectangle expand_icon_bound = (Rectangle){{
            widget_bound(widget).x + current,
            widget_bound(widget).y,
            bitmap_bound(widget->icon_expand).width,
            widget_bound(widget).height,
        }};

        painter_blit_icon(
            painter,
            widget->icon_expand,
            rectangle_center_within(
                bitmap_bound(widget->icon_expand),
                expand_icon_bound),
            widget_get_color(widget, THEME_FOREGROUND));

        current += expand_icon_bound.width;
    }

    for (size_t i = 0; i < path_element_count(widget->path); i++)
    {
        const char *element = path_peek_at(widget->path, i);

        int text_width = font_measure_string(widget_font(), element);

        Rectangle element_bound = (Rectangle){{
            widget_bound(widget).x + current,
            widget_bound(widget).y,
            text_width,
            widget_bound(widget).height,
        }};

        painter_draw_string(
            painter,
            widget_font(),
            element,
            vec2i_add(element_bound.position, vec2i(0, 20)),
            widget_get_color(widget, THEME_FOREGROUND));

        current += text_width;

        if (i != path_element_count(widget->path) - 1)
        {
            Rectangle expand_icon_bound = (Rectangle){{
                widget_bound(widget).x + current,
                widget_bound(widget).y,
                bitmap_bound(widget->icon_expand).width,
                widget_bound(widget).height,
            }};

            painter_blit_icon(
                painter,
                widget->icon_expand,
                rectangle_center_within(
                    bitmap_bound(widget->icon_expand),
                    expand_icon_bound),
                widget_get_color(widget, THEME_FOREGROUND));

            current += expand_icon_bound.width;
        }
    }
}

void breadcrumb_destroy(Breadcrumb *widget)
{
    path_destroy(widget->path);
}

Widget *breadcrumb_create(Widget *parent, const char *current_path)
{
    Breadcrumb *widget = __create(Breadcrumb);

    WIDGET(widget)->paint = (WidgetPaintCallback)breadcrumb_paint;
    WIDGET(widget)->destroy = (WidgetDestroyCallback)breadcrumb_destroy;

    widget->path = path_create(current_path);
    widget->icon_computer = icon_cache_get_icon("laptop");
    widget->icon_expand = icon_cache_get_icon("chevron-right");

    widget_initialize(WIDGET(widget), "Breadcrumb", parent);

    return WIDGET(widget);
}

void breadcrumb_navigate(Widget *widget, Path *path)
{
    Breadcrumb *breadcrumb = (Breadcrumb *)widget;

    path_destroy(breadcrumb->path);
    breadcrumb->path = path_clone(path);

    widget_update(widget);
}