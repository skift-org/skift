#pragma once

#include <libwidget/Model.h>
#include <libwidget/Widget.h>
#include <libwidget/widgets/ScrollBar.h>

struct Table : public Widget
{
    Model *model;
    int selected;
    int scroll_offset;

    ScrollBar *scrollbar;
};

Table *table_create(Widget *parent, Model *model);

void table_update_model(Table *table);
