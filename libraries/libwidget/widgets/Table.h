#pragma once

#include <libwidget/Model.h>
#include <libwidget/Widget.h>

struct Table
{
    Widget widget;
    Model *model;
    int selected;
    int scroll_offset;

    Widget *scrollbar;
};

Widget *table_create(Widget *parent, Model *model);

void table_update_model(Table *table);
