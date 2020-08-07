#pragma once

#include <libwidget/Widget.h>

struct Separator : public Widget
{
};

Separator *separator_create(Widget *parent);
