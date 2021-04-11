#pragma once

#include <libgraphic/Font.h>
#include <libio/Terminal.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libterminal/Terminal.h>
#include <libwidget/Component.h>

class TerminalWidget : public Widget::Component
{
private:
    OwnPtr<Terminal::Terminal> _terminal;
    bool _cursor_blink;
    int _scroll_offset = 0;

    IO::Terminal _terminal_device;

    OwnPtr<Timer> _cursor_blink_timer;
    OwnPtr<Notifier> _server_notifier;

public:
    void blink() { _cursor_blink = !_cursor_blink; };

    TerminalWidget(Component *parent);

    void handle_read();

    void paint(Graphic::Painter &, const Math::Recti &) override;

    void event(Widget::Event *event) override;

    void do_layout() override;
};
