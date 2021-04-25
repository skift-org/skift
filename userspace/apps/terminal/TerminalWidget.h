#pragma once

#include <libasync/Notifier.h>
#include <libasync/Timer.h>
#include <libgraphic/Font.h>
#include <libio/Terminal.h>
#include <libterminal/Terminal.h>
#include <libwidget/Element.h>

class TerminalWidget : public Widget::Element
{
private:
    OwnPtr<Terminal::Terminal> _terminal;
    bool _cursor_blink;
    int _scroll_offset = 0;

    IO::Terminal _terminal_device;

    OwnPtr<Async::Timer> _cursor_blink_timer;
    OwnPtr<Async::Notifier> _server_notifier;

public:
    void blink() { _cursor_blink = !_cursor_blink; };

    TerminalWidget();

    void handle_read();

    void paint(Graphic::Painter &, const Math::Recti &) override;

    void event(Widget::Event *event) override;

    void do_layout() override;
};
