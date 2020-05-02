#include <libgraphic/Painter.h>
#include <libwidget/Event.h>
#include <libwidget/ScrollBar.h>

Rectangle scrollbar_button_down(ScrollBar *widget)
{
    return rectangle_bottom(widget_bound(widget), widget_bound(widget).width);
}

Rectangle scrollbar_button_up(ScrollBar *widget)
{
    return rectangle_top(widget_bound(widget), widget_bound(widget).width);
}

static Rectangle scrollbar_track(ScrollBar *widget)
{
    return rectangle_shrink(widget_bound(widget), INSETS(widget_bound(widget).width, 0));
}

static Rectangle scrollbar_thumb(ScrollBar *widget)
{
    Rectangle track = scrollbar_track(widget);

    int thumb_height = track.height * (widget->thumb / (double)widget->track);
    int thump_position = track.height * (widget->value / (double)widget->track);

    return (Rectangle){{
        track.X,
        track.Y + thump_position,
        track.width,
        thumb_height,
    }};
}

static void scrollbar_paint(ScrollBar *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter_clear_rectangle(painter, widget_bound(widget), widget_get_color(widget, THEME_MIDDLEGROUND));
    painter_clear_rectangle(painter, scrollbar_thumb(widget), widget_get_color(widget, THEME_MIDDLEGROUND));
    painter_draw_rectangle(painter, scrollbar_thumb(widget), widget_get_color(widget, THEME_BORDER));
}

static void scrollbar_scroll_to(ScrollBar *widget, Point mouse_position)
{
    int new_value = mouse_position.Y - scrollbar_track(widget).Y;
    widget->value = (new_value / (double)scrollbar_track(widget).height) * widget->track - widget->thumb / 2;

    widget->value = clamp(widget->value, 0, widget->track - widget->thumb);

    widget_dispatch_event(WIDGET(widget), EVENT_NO_ARGS(EVENT_VALUE_CHANGE));
    widget_update(WIDGET(widget));
}

static void scrollbar_scroll_thumb(ScrollBar *widget, Point mouse_position)
{
    mouse_position = point_sub(mouse_position, widget->mouse_origine);

    int new_value = mouse_position.Y - scrollbar_track(widget).Y;
    widget->value = (new_value / (double)scrollbar_track(widget).height) * widget->track;

    widget->value = clamp(widget->value, 0, widget->track - widget->thumb);

    widget_dispatch_event(WIDGET(widget), EVENT_NO_ARGS(EVENT_VALUE_CHANGE));
    widget_update(WIDGET(widget));
}

static void scrollbar_event(ScrollBar *widget, Event *event)
{
    if (is_mouse_event(event))
    {
        MouseEvent *mouse_event = (MouseEvent *)event;

        if (event->type == EVENT_MOUSE_MOVE && mouse_event->buttons & MOUSE_BUTTON_LEFT)
        {
            scrollbar_scroll_thumb(widget, mouse_event->position);
        }
        else if (event->type == EVENT_MOUSE_BUTTON_PRESS)
        {
            if (!rectangle_containe_point(scrollbar_thumb(widget), mouse_event->position))
            {
                scrollbar_scroll_to(widget, mouse_event->position);
            }

            widget->mouse_origine = point_sub(mouse_event->position, scrollbar_thumb(widget).position);
        }

        event->accepted = true;
    }
}

Widget *scrollbar_create(Widget *parent)
{
    ScrollBar *scrollbar = __create(ScrollBar);

    scrollbar->track = 1024;
    scrollbar->value = 512;
    scrollbar->thumb = 128;

    WIDGET(scrollbar)->paint = (WidgetPaintCallback)scrollbar_paint;
    WIDGET(scrollbar)->event = (WidgetEventCallback)scrollbar_event;

    widget_initialize(WIDGET(scrollbar), "ScrollBar", parent);

    return WIDGET(scrollbar);
}
