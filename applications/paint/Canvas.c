#include <libgraphic/Painter.h>

#include "paint/Canvas.h"

#define CHECKER_SIZE 8

static void checker_board(Painter *painter, Rectangle rectangle)
{
    for (int x = 0; x < rectangle.width / CHECKER_SIZE + 1; x++)
    {
        for (int y = 0; y < rectangle.height / CHECKER_SIZE + 1; y++)
        {
            Rectangle cell = (Rectangle){
                {
                    rectangle.X + x * CHECKER_SIZE,
                    rectangle.Y + y * CHECKER_SIZE,
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
    Rectangle bound = bitmap_bound(widget->bitmap);

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
        widget->bitmap,
        bitmap_bound(widget->bitmap), destination);
}

void canvas_event(Canvas *widget, Event *event)
{
    if (event->type == EVENT_MOUSE_MOVE)
    {
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (mouse_event->buttons & MOUSE_BUTTON_LEFT)
        {
            Point from = point_sub(mouse_event->old_position, canvas_bound(widget).position);
            Point to = point_sub(mouse_event->position, canvas_bound(widget).position);

            painter_draw_line(widget->painter, from, to, COLOR_RED);

            widget_update(WIDGET(widget));
        }
    }
}

Widget *canvas_create(Widget *parent, int width, int height)
{
    Canvas *canvas = __create(Canvas);

    canvas->bitmap = bitmap_create(width, height);
    canvas->painter = painter_create(canvas->bitmap);

    painter_clear(canvas->painter, COLOR_RGBA(0, 0, 0, 0));

    WIDGET(canvas)->paint = (WidgetPaintCallback)canvas_paint;
    WIDGET(canvas)->event = (WidgetEventCallback)canvas_event;

    widget_initialize(WIDGET(canvas), "Canvas", parent);

    return WIDGET(canvas);
}