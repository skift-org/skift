#pragma once

#include <libwidget/Widget.h>

Widget *toolbar_create(Widget *parent);

Widget *toolbar_icon_create(Widget *parent, const char *icon);

Widget *toolbar_icon_with_text_create(Widget *parent, const char *icon, const char *text);