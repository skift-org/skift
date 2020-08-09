#pragma once

#include <libwidget/Widget.h>

struct Panel : public Widget
{
};

Panel *panel_create(Widget *parent);
