#pragma once

#include <libsystem/RingBuffer.h>
#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;

    Color color;
    double *data;
    size_t data_size;
    size_t current;
} GraphWidget;

void graph_widget_record(GraphWidget *widget, double data);

Widget *graph_widget_create(Widget *parent, Color color);
