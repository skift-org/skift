#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

struct IconPanel
{
    Widget widget;
    RefPtr<Icon> icon;
};

Widget *icon_panel_create(Widget *parent, RefPtr<Icon> icon);
