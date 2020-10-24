#pragma once

#include <libgraphic/Font.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Stream.h>
#include <libterminal/Terminal.h>
#include <libwidget/Widget.h>

class TerminalWidget : public Widget
{
private:
    terminal::Terminal *_terminal;
    bool _cursor_blink;

    Stream *_server_stream;
    Stream *_client_stream;

    OwnPtr<Timer> _cursor_blink_timer;
    Notifier *_server_notifier;

public:
    terminal::Terminal *terminal() { return _terminal; }

    void blink() { _cursor_blink = !_cursor_blink; };

    TerminalWidget(Widget *parent);

    ~TerminalWidget();

    void paint(Painter &painter, Rectangle rectangle);

    void event(Event *event);

    void do_layout();
};
