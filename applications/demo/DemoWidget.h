#pragma once

#include <libwidget/Widget.h>

#include "demo/Demos.h"

struct DemoWidget
{
    Widget widget;

    double time;
    Demo *demo;

    RefPtr<Bitmap> bitmap;
    Painter painter;

    Timer *timer;
} ;

void demo_widget_set_demo(Widget *widget, Demo *demo);

Widget *demo_widget_create(Widget *parent);
