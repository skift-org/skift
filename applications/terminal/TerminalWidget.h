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

class TerminalWidget : public Widget
{
private:
    Terminal *_terminal;
    bool _cursor_blink;

    Stream *_master_stream;
    Stream *_slave_stream;

    OwnPtr<Timer> _cursor_blink_timer;
    Notifier *_master_notifier;

public:
    Terminal *terminal() { return _terminal; }

    void blink() { _cursor_blink = !_cursor_blink; };

    TerminalWidget(Widget *parent);

    ~TerminalWidget();

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);

    void do_layout();
};

Widget *terminal_widget_create(Widget *parent);
