#pragma once

#include <libwidget/Widget.h>

#include "demo/Demos.h"

typedef struct
{
    Widget widget;

    double time;
    Demo *demo;
    Bitmap *bitmap;
    Painter *painter;
    Timer *timer;
} DemoWidget;

void demo_widget_set_demo(Widget *widget, Demo *demo);

Widget *demo_widget_create(Widget *parent);