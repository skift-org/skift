#pragma once

#include <libwidget/Widget.h>

typedef enum
{
    BUTTON_IDLE,
    BUTTON_OVER,
    BUTTON_PRESS,
} ButtonState;

typedef enum
{
    BUTTON_TEXT,
    BUTTON_OUTLINE,
    BUTTON_FILLED,
} ButtonStyle;

typedef struct
{
    Widget widget;
    ButtonState state;
    ButtonStyle style;
} Button;

Widget *button_create(Widget *parent, ButtonStyle style);

Widget *button_create_with_text(Widget *parent, ButtonStyle style, const char *text);

Widget *button_create_with_icon(Widget *parent, ButtonStyle style, const char *icon);

Widget *button_create_with_icon_and_text(Widget *parent, ButtonStyle style, const char *icon, const char *text);
