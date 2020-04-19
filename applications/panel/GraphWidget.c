#include <libgraphic/Painter.h>
#include <libsystem/logger.h>

#include "panel/GraphWidget.h"

static double distance(int from, int to, int size)
{
    if (from > to)
    {
        from -= size;
    }

    return abs(to - from) / (double)size;
}

void graph_widget_paint(GraphWidget *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter_fill_rectangle(painter, widget_bound(widget), widget_get_color(widget, THEME_MIDDLEGROUND));

    for (size_t i = 0; i < widget->data_size; i++)
    {
        Rectangle bar = (Rectangle){{
            widget_bound(widget).X + i,
            widget_bound(widget).Y + widget_bound(widget).height * (1.0 - widget->data[i]),
            1,
            widget_bound(widget).height,
        }};

        double dist = (1 - distance(i, widget->current - 1, widget->data_size)) * 0.5;

        painter_fill_rectangle(painter, bar, ALPHA(widget->color, dist));
        painter_plot_pixel(painter, bar.position, widget->color);
    }

    Rectangle cursor = (Rectangle){{
        widget_bound(widget).X + widget->current,
        widget_bound(widget).Y,
        1,
        widget_bound(widget).height,
    }};

    painter_fill_rectangle(painter, cursor, widget_get_color(widget, THEME_BORDER));
}

Point graph_widget_size(GraphWidget *widget)
{
    return (Point){widget->data_size, 10};
}

void graph_widget_destroy(GraphWidget *widget)
{
    free(widget->data);
}

void graph_widget_record(GraphWidget *widget, double data)
{
    widget->data[widget->current] = data;
    widget->current = (widget->current + 1) % widget->data_size;

    widget_update(WIDGET(widget));
}

Widget *graph_widget_create(Widget *parent, Color color)
{
    GraphWidget *widget = __create(GraphWidget);

    widget->color = color;
    widget->data = (double *)calloc(50, sizeof(double));
    widget->data_size = 50;
    widget->current = 0;

    WIDGET(widget)->paint = (WidgetPaintCallback)graph_widget_paint;
    WIDGET(widget)->size = (WidgetComputeSizeCallback)graph_widget_size;
    WIDGET(widget)->destroy = (WidgetDestroyCallback)graph_widget_destroy;

    widget_initialize(WIDGET(widget), "Graph", parent);

    return WIDGET(widget);
}