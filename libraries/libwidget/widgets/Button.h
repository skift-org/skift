#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Widget.h>

enum ButtonState
{
    BUTTON_IDLE,
    BUTTON_OVER,
    BUTTON_PRESS,
};

enum ButtonStyle
{
    BUTTON_TEXT,
    BUTTON_OUTLINE,
    BUTTON_FILLED,
};

struct Button : public Widget
{
    ButtonState state;
    ButtonStyle style;
};

Widget *button_create(Widget *parent, ButtonStyle style);

Widget *button_create_with_text(Widget *parent, ButtonStyle style, const char *text);

Widget *button_create_with_icon(Widget *parent, ButtonStyle style, RefPtr<Icon> icon);

Widget *button_create_with_icon_and_text(Widget *parent, ButtonStyle style, RefPtr<Icon> icon, const char *text);
