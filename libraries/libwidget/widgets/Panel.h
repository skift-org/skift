#pragma once

#include <libwidget/Widget.h>

struct Panel : public Widget
{
};

Widget *panel_create(Widget *parent);
