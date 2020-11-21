#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

Widget *toolbar_create(Widget *parent);

Widget *toolbar_icon_create(Widget *parent, RefPtr<Icon> icon);

Widget *toolbar_icon_with_text_create(Widget *parent, RefPtr<Icon> icon, const char *text);
