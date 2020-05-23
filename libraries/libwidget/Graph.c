#include <libgraphic/Painter.h>
#include <libsystem/assert.h>
#include <libsystem/logger.h>
#include <libwidget/Graph.h>

static double distance(double from, double to, int size)
{
    if (from > to)
    {
        from -= size;
    }

    return abs(to - from) / (double)size;
}

double graph_sample(Graph *widget, float where)
{
    assert(where >= 0 && where <= 1);

    return widget->data[(size_t)(widget->data_size * where)];
}

void graph_paint(Graph *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter_fill_rectangle(painter, widget_get_bound(widget), widget_get_color(widget, THEME_MIDDLEGROUND));

    double cursor_position = widget->current / (double)widget->data_size;

    for (int i = 0; i < widget_get_bound(widget).width; i++)
    {
        double where = i / (double)widget_get_bound(widget).width;
        double data = graph_sample(widget, where);

        Rectangle bar = (Rectangle){{
            widget_get_bound(widget).x + i,
            widget_get_bound(widget).y + widget_get_bound(widget).height * (1.0 - data),
            1,
            widget_get_bound(widget).height,
        }};

        double dist = (1 - distance(where, cursor_position, 1)) * 0.5;

        painter_fill_rectangle(painter, bar, ALPHA(widget->color, dist));
        painter_plot_pixel(painter, bar.position, widget->color);
    }

    Rectangle cursor = (Rectangle){{
        widget_get_bound(widget).x + widget_get_bound(widget).width * cursor_position,
        widget_get_bound(widget).y,
        1,
        widget_get_bound(widget).height,
    }};

    painter_fill_rectangle(painter, cursor, widget_get_color(widget, THEME_BORDER));
}

Vec2i graph_size(Graph *widget)
{
    return vec2i(widget->data_size, 100);
}

void graph_record(Graph *widget, double data)
{
    widget->data[widget->current] = data;
    widget->current = (widget->current + 1) % widget->data_size;

    widget_update(WIDGET(widget));
}

void graph_destroy(Graph *widget)
{
    free(widget->data);
}

static const WidgetClass graph_class = {
    .name = "Graph",

    .paint = (WidgetPaintCallback)graph_paint,
    .size = (WidgetComputeSizeCallback)graph_size,
    .destroy = (WidgetDestroyCallback)graph_destroy,
};

Widget *graph_create(Widget *parent, size_t data_size, Color color)
{
    Graph *widget = __create(Graph);

    widget->color = color;
    widget->data = (double *)calloc(data_size, sizeof(double));
    widget->data_size = data_size;
    widget->current = 0;

    widget_initialize(WIDGET(widget), &graph_class, parent);

    return WIDGET(widget);
}