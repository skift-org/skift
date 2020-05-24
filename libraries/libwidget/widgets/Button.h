#pragma once

#include <libwidget/Widget.h>

typedef enum
{
    BUTTON_IDLE,
    BUTTON_OVER,
    BUTTON_PRESS,
} ButtonState;

typedef struct
{
    Widget widget;
    ButtonState state;
} Button;

Widget *button_create(Widget *parent);

Widget *button_create_with_text(Widget *parent, const char *text);

Widget *button_create_with_icon(Widget *parent, const char *icon);

Widget *button_create_with_icon_and_text(Widget *parent, const char *icon, const char *text);
