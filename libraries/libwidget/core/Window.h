#pragma once

#include <libsystem/runtime.h>
#include <libwidget/core/Widget.h>

typedef struct
{
    Widget widget;

    int handle;
} Window;

Widget *window_create(void);

void window_destroy(Widget *window);
