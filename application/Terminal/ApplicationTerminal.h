#pragma once

#include <libgraphic/Font.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Stream.h>
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
    bool cursor_blink;

    Stream *master_stream;
    Stream *slave_stream;

    Timer *cursor_blink_timer;
    Notifier *master_notifier;
} TerminalWidget;

Widget *terminal_widget_create(Widget *parent);