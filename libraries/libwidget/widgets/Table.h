#pragma once

#include <libwidget/Model.h>
#include <libwidget/Widget.h>

struct Table : public Widget
{
    Model *model;
    int selected;
    int scroll_offset;

    Widget *scrollbar;
};

Table *table_create(Widget *parent, Model *model);

void table_update_model(Table *table);
