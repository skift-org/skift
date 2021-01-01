#pragma once

#include <libwidget/Window.h>

Window *window_create_from_file(const char *path);

Widget *widget_create_from_file(Widget *parent, const char *path);