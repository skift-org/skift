#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>

#include "demo/DemoWidget.h"

void demo_widget_paint(DemoWidget *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget->bitmap == NULL)
    {
        widget->bitmap = bitmap_create(widget_bound(widget).width, widget_bound(widget).height);
        widget->painter = painter_create(widget->bitmap);
        painter_clear(widget->painter, COLOR_BLACK);
    }

    if (widget_bound(widget).width != bitmap_bound(widget->bitmap).width ||
        widget_bound(widget).width != bitmap_bound(widget->bitmap).width)
    {
        painter_destroy(widget->painter);
        bitmap_destroy(widget->bitmap);

        widget->bitmap = bitmap_create(widget_bound(widget).width, widget_bound(widget).height);
        widget->painter = painter_create(widget->bitmap);
        painter_clear(widget->painter, COLOR_BLACK);
    }

    if (widget->demo)
    {
        widget->demo->callback(widget->painter, bitmap_bound(widget->bitmap), widget->time);

        painter_draw_string(widget->painter, widget_font(), widget->demo->name, (Point){9, 17}, COLOR_BLACK);
        painter_draw_string(widget->painter, widget_font(), widget->demo->name, (Point){8, 16}, COLOR_WHITE);
    }

    painter_blit_bitmap_no_alpha(painter, widget->bitmap, bitmap_bound(widget->bitmap), widget_bound(widget));
}

void demo_widget_on_timer_tick(DemoWidget *widget)
{
    widget->time += 1.0 / 60;
    widget_update(WIDGET(widget));
}

void demo_widget_set_demo(Widget *widget, Demo *demo)
{
    if (((DemoWidget *)widget)->painter)
    {
        painter_clear(((DemoWidget *)widget)->painter, COLOR_BLACK);
    }
    
    ((DemoWidget *)widget)->demo = demo;
}

void demo_widget_destroy(DemoWidget *widget)
{
    timer_destroy(widget->timer);
    painter_destroy(widget->painter);
    bitmap_destroy(widget->bitmap);
}

Widget *demo_widget_create(Widget *parent)
{
    DemoWidget *widget = __create(DemoWidget);

    widget_initialize(WIDGET(widget), "Demo", parent);

    WIDGET(widget)->destroy = (WidgetDestroyCallback)demo_widget_destroy;
    WIDGET(widget)->paint = (WidgetPaintCallback)demo_widget_paint;

    widget->demo = NULL;
    widget->timer = timer_create(widget, 1000 / 60, (TimerCallback)demo_widget_on_timer_tick);
    timer_start(widget->timer);

    return WIDGET(widget);
}
