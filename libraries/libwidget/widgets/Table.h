#pragma once

#include <libwidget/Model.h>
#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
    Model *model;
    int selected;
    int scroll_offset;

    Widget *scrollbar;
} Table;

Widget *table_create(Widget *parent, Model *model);

void table_update_model(Table *table);
