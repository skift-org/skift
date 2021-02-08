#pragma once

#include <libwidget/Window.h>

Window *window_create_from_file(String path);

Widget *widget_create_from_file(Widget *parent, String path);