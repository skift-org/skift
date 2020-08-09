#pragma once

#include <libsystem/utils/RingBuffer.h>
#include <libwidget/Widget.h>

struct Graph : public Widget
{
    Color color;
    double *data;
    size_t data_size;
    size_t current;
};

void graph_record(Graph *widget, double data);

Graph *graph_create(Widget *parent, size_t data_size, Color color);
