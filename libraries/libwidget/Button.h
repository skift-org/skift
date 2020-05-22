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
    char *text;
} Button;

Widget *button_create(Widget *parent, const char *text);