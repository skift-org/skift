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
} Graph;

void graph_record(Graph *widget, double data);

Widget *graph_create(Widget *parent, size_t data_size, Color color);
