#pragma once

#include <libasync/Notifier.h>
#include <libasync/Timer.h>
#include <libgraphic/Font.h>
#include <libio/Terminal.h>
#include <libterminal/Terminal.h>
#include <libwidget/Element.h>

struct TerminalView : public Widget::Element
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

    TerminalView();

    Graphic::Color cell_color(Terminal::Color terminal_color);

    Math::Recti cell_bound(int x, int y)
    {
        return {
            Math::Vec2i(x, y) * cell_size(),
            cell_size(),
        };
    }

    Math::Vec2i cell_size()
    {
        return {7, 16};
    }

    void handle_read();

    void paint_cell(
        Graphic::Painter &painter,
        int x,
        int y,
        Text::Rune rune,
        Terminal::Color foreground,
        Terminal::Color background,
        Terminal::Attributes attributes);

    void paint_cell(Graphic::Painter &painter, int x, int y, Terminal::Cell cell)
    {
        paint_cell(painter, x, y, cell.rune, cell.attributes.foreground, cell.attributes.background, cell.attributes);
    }

    void paint(Graphic::Painter &, const Math::Recti &) override;

    void event(Widget::Event *event) override;

    void layout() override;

    Math::Vec2i size() override
    {
        return Math::Vec2i{80, 25} * Math::Vec2i{7, 16};
    }
};

WIDGET_BUILDER(TerminalView, terminal_view);