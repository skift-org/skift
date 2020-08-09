#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

struct IconPanel : public Widget
{
    RefPtr<Icon> icon;
};

IconPanel *icon_panel_create(Widget *parent, RefPtr<Icon> icon);
