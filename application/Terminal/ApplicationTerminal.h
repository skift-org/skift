#pragma once

#include <libgraphic/Font.h>
#include <libterminal/Terminal.h>
#include <libwidget/Widget.h>

struct TerminalWidget;

typedef struct
{
    TerminalRenderer renderer;

    struct TerminalWidget *widget;
} TerminalWidgetRenderer;

typedef struct TerminalWidget
{
    Widget widget;

    Terminal *terminal;
    Font *mono_font;
} TerminalWidget;

Widget *terminal_widget_create(Widget *parent);