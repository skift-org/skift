#include <libgraphic/Painter.h>

#include "paint/Canvas.h"
#include "paint/PaintTool.h"

#define CHECKER_SIZE 8

static void checker_board(Painter *painter, Rectangle rectangle)
{
    for (int x = 0; x < rectangle.width / CHECKER_SIZE + 1; x++)
    {
        for (int y = 0; y < rectangle.height / CHECKER_SIZE + 1; y++)
        {
            Rectangle cell = (Rectangle){
                {
                    rectangle.x + x * CHECKER_SIZE,
                    rectangle.y + y * CHECKER_SIZE,
                    CHECKER_SIZE,
                    CHECKER_SIZE,
                }};

            cell = rectangle_clip(cell, rectangle);

            if ((x + y) % 2 == 0)
                painter_fill_rectangle(painter, cell, COLOR_WHITE);
            else
                painter_fill_rectangle(painter, cell, COLOR_GAINSBORO);
        }
    }
}

Rectangle canvas_bound(Canvas *widget)
{

    Rectangle bound = bitmap_bound(widget->document->bitmap);

    bound = rectangle_offset(bound, widget_bound(widget).position);

    bound = rectangle_center_within(bound, widget_bound(widget));

    return bound;
}

void canvas_paint(Canvas *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    Rectangle destination = canvas_bound(widget);

    checker_board(painter, destination);
    painter_draw_rectangle(painter, destination, widget_get_color(widget, THEME_BORDER));

    painter_blit_bitmap(
        painter,
        widget->document->bitmap,
        bitmap_bound(widget->document->bitmap), destination);
}

void canvas_event(Canvas *widget, Event *event)
{
    if (is_mouse_event(event))
    {
        Event event_copy = *event;

        event_copy.mouse.old_position = vec2i_sub(event_copy.mouse.old_position, canvas_bound(widget).position);
        event_copy.mouse.position = vec2i_sub(event_copy.mouse.position, canvas_bound(widget).position);

        if (widget->document->tool->on_mouse_event)
            widget->document->tool->on_mouse_event(widget->document->tool, widget->document, event_copy);

        if (widget->document->dirty)
        {
            widget_update(WIDGET(widget));
            widget->document->dirty = false;
        }
    }
}

Widget *canvas_create(Widget *parent, PaintDocument *document)
{
    Canvas *canvas = __create(Canvas);

    WIDGET(canvas)->paint = (WidgetPaintCallback)canvas_paint;
    WIDGET(canvas)->event = (WidgetEventCallback)canvas_event;

    canvas->document = document;

    widget_initialize(WIDGET(canvas), "Canvas", parent);

    return WIDGET(canvas);
}