#pragma once

#include <libwidget/Model.h>
#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
    Model *model;
    int selected;
} Table;

Widget *table_create(Widget *parent, Model *model);
