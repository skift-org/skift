#include <libgraphic/Painter.h>

#include "paint/Canvas.h"
#include "paint/PaintTool.h"

Rectangle canvas_bound(Canvas *widget)
{
    Rectangle bound = widget->document->bitmap->bound();
    bound = bound.centered_within(widget_get_bound(widget));
    return bound;
}

void canvas_paint(Canvas *widget, Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    Rectangle destination = canvas_bound(widget);

    painter.fill_checkboard(destination, 8, COLOR_WHITE, COLOR_GAINSBORO);
    painter.draw_rectangle(destination, widget_get_color(widget, THEME_BORDER));

    painter.blit_bitmap(
        *widget->document->bitmap,
        widget->document->bitmap->bound(), destination);
}

void canvas_event(Canvas *widget, Event *event)
{
    if (is_mouse_event(event))
    {
        Event event_copy = *event;

        event_copy.mouse.old_position = event_copy.mouse.old_position - canvas_bound(widget).position();
        event_copy.mouse.position = event_copy.mouse.position - canvas_bound(widget).position();

        if (widget->document->tool->on_mouse_event)
            widget->document->tool->on_mouse_event(widget->document->tool, widget->document, event_copy);

        if (widget->document->dirty)
        {
            widget->should_repaint();
            widget->document->dirty = false;
        }
    }
}

static const WidgetClass canvas_class = {
    .name = "Canvas",

    .paint = (WidgetPaintCallback)canvas_paint,
    .event = (WidgetEventCallback)canvas_event,
};

Widget *canvas_create(Widget *parent, PaintDocument *document)
{
    Canvas *canvas = __create(Canvas);

    canvas->document = document;

    widget_initialize(canvas, &canvas_class, parent);

    return canvas;
}
