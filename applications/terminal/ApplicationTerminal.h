#pragma once

#include <libgraphic/Font.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Stream.h>
#include <libterminal/Terminal.h>
#include <libwidget/Widget.h>

struct TerminalWidget;

struct TerminalWidgetRenderer
{
    TerminalRenderer renderer;

    struct TerminalWidget *widget;
};

struct TerminalWidget : public Widget
{
    Terminal *terminal;
    bool cursor_blink;

    Stream *master_stream;
    Stream *slave_stream;

    Timer *cursor_blink_timer;
    Notifier *master_notifier;
};

Widget *terminal_widget_create(Widget *parent);
