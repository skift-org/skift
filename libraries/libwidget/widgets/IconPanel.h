#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

struct IconPanel : public Widget
{
    RefPtr<Icon> icon;
};

Widget *icon_panel_create(Widget *parent, RefPtr<Icon> icon);
