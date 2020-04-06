#pragma once

#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>
#include <libwidget/Widget.h>

typedef void (*DrawDemoCallback)(Painter *painter, Rectangle screen, double time);

typedef struct
{
    Widget widget;

    double time;
    DrawDemoCallback demo;
    Bitmap *bitmap;
    Painter *painter;
    Timer *timer;
} DemoWidget;

void demo_widget_paint(DemoWidget *widget, Painter *painter)
{
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

    widget->demo(widget->painter, bitmap_bound(widget->bitmap), widget->time);

    painter_blit_bitmap(painter, widget->bitmap, bitmap_bound(widget->bitmap), widget_bound(widget));
}

void demo_widget_on_timer_tick(DemoWidget *widget)
{
    widget->time += 1.0 / 60;
    widget_update(WIDGET(widget));
}

void demo_widget_destroy(DemoWidget *widget)
{
    timer_destroy(widget->timer);
    painter_destroy(widget->painter);
    bitmap_destroy(widget->bitmap);
}

Widget *demo_widget_create(Widget *parent, DrawDemoCallback demo)
{
    DemoWidget *widget = __create(DemoWidget);

    widget_initialize(WIDGET(widget), "Demo", parent, RECTANGLE_SIZE(16, 16));

    WIDGET(widget)->destroy = (WidgetDestroyCallback)demo_widget_destroy;
    WIDGET(widget)->paint = (WidgetPaintCallback)demo_widget_paint;

    widget->demo = demo;
    widget->timer = timer_create(widget, 1000 / 30, (TimerCallback)demo_widget_on_timer_tick);
    timer_start(widget->timer);

    return WIDGET(widget);
}

static int demo_start(int argc, char **argv, const char *name, DrawDemoCallback demo)
{
    Result result = application_initialize(argc, argv);

    if (result != SUCCESS)
    {
        return -1;
    }

    Window *main_window = window_create(name, 500, 400);

    demo_widget_create(window_root(main_window), demo);

    application_run();

    return 0;
}