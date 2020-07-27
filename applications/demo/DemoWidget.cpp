#include <libsystem/eventloop/Timer.h>
#include <libwidget/Application.h>

#include "demo/DemoWidget.h"

void demo_widget_paint(DemoWidget *widget, Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget->bitmap == nullptr)
    {
        widget->bitmap = bitmap_create(widget_get_bound(widget).width(), widget_get_bound(widget).height());
        widget->painter = new Painter(widget->bitmap);
        widget->painter->clear(COLOR_BLACK);
    }

    if (widget_get_bound(widget).width() != bitmap_bound(widget->bitmap).width() ||
        widget_get_bound(widget).height() != bitmap_bound(widget->bitmap).height())
    {
        bitmap_destroy(widget->bitmap);

        widget->bitmap = bitmap_create(widget_get_bound(widget).width(), widget_get_bound(widget).height());
        widget->painter = new Painter(widget->bitmap);
        widget->painter->clear(COLOR_BLACK);
    }

    if (widget->demo)
    {
        widget->demo->callback(*widget->painter, bitmap_bound(widget->bitmap), widget->time);
    }

    painter.blit_bitmap_no_alpha(widget->bitmap, bitmap_bound(widget->bitmap), widget_get_bound(widget));

    painter.draw_string(widget_font(), widget->demo->name, widget_get_bound(widget).position() + Vec2i(9, 17), COLOR_BLACK);
    painter.draw_string(widget_font(), widget->demo->name, widget_get_bound(widget).position() + Vec2i(8, 16), COLOR_WHITE);
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
        ((DemoWidget *)widget)->painter->clear(COLOR_BLACK);
    }

    ((DemoWidget *)widget)->demo = demo;
}

void demo_widget_destroy(DemoWidget *widget)
{
    timer_destroy(widget->timer);
    delete widget->painter;
    bitmap_destroy(widget->bitmap);
}

static const WidgetClass demo_class = {
    .name = "Demo",

    .destroy = (WidgetDestroyCallback)demo_widget_destroy,
    .paint = (WidgetPaintCallback)demo_widget_paint,
};

Widget *demo_widget_create(Widget *parent)
{
    DemoWidget *widget = __create(DemoWidget);

    widget_initialize(WIDGET(widget), &demo_class, parent);

    widget->demo = nullptr;
    widget->timer = timer_create(widget, 1000 / 60, (TimerCallback)demo_widget_on_timer_tick);
    timer_start(widget->timer);

    return WIDGET(widget);
}
