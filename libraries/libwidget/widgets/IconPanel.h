#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
    RefPtr<Icon> icon;
} IconPanel;

Widget *icon_panel_create(Widget *parent, RefPtr<Icon> icon);
