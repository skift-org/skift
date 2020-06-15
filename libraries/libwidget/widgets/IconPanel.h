#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

typedef struct
{
    Widget widget;
    Icon *icon;
} IconPanel;

Widget *icon_panel_create(Widget *parent, Icon *icon);
