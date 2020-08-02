#pragma once

#include <libsystem/io/Path.h>
#include <libwidget/Widget.h>

struct Breadcrumb
{
    Widget widget;

    Path *path;

    RefPtr<Icon> icon_computer;
    RefPtr<Icon> icon_expand;
};

Widget *breadcrumb_create(Widget *parent, const char *current_path);

void breadcrumb_navigate(Widget *widget, Path *path);
