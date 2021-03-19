#pragma once

#include <libgraphic/Font.h>
#include <libio/Terminal.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libterminal/Terminal.h>
#include <libwidget/Widget.h>
class TerminalWidget : public Widget
{
private:
    OwnPtr<terminal::Terminal> _terminal;
    bool _cursor_blink;

    IO::Terminal _terminal_device;

    OwnPtr<Timer> _cursor_blink_timer;
    OwnPtr<Notifier> _server_notifier;

public:
    void blink() { _cursor_blink = !_cursor_blink; };

    TerminalWidget(Widget *parent);

    void handle_read();

    void paint(Painter &, const Recti &) override;

    void event(Event *event) override;

    void do_layout() override;
};
