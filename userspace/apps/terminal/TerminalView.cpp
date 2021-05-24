#include <libgraphic/Painter.h>
#include <libio/Streams.h>
#include <libsystem/process/Launchpad.h>
#include <libutils/Assert.h>
#include <libwidget/Event.h>
#include <libwidget/Window.h>

#include "terminal/TerminalView.h"

#define TERMINAL_IO_BUFFER_SIZE 4096

static Widget::ThemeColorRole _color_to_role[Terminal::_COLOR_COUNT] = {
    [Terminal::BLACK] = Widget::THEME_ANSI_BLACK,
    [Terminal::RED] = Widget::THEME_ANSI_RED,
    [Terminal::GREEN] = Widget::THEME_ANSI_GREEN,
    [Terminal::YELLOW] = Widget::THEME_ANSI_YELLOW,
    [Terminal::BLUE] = Widget::THEME_ANSI_BLUE,
    [Terminal::MAGENTA] = Widget::THEME_ANSI_MAGENTA,
    [Terminal::CYAN] = Widget::THEME_ANSI_CYAN,
    [Terminal::GREY] = Widget::THEME_ANSI_WHITE,
    [Terminal::BRIGHT_BLACK] = Widget::THEME_ANSI_BRIGHT_BLACK,
    [Terminal::BRIGHT_RED] = Widget::THEME_ANSI_BRIGHT_RED,
    [Terminal::BRIGHT_GREEN] = Widget::THEME_ANSI_BRIGHT_GREEN,
    [Terminal::BRIGHT_YELLOW] = Widget::THEME_ANSI_BRIGHT_YELLOW,
    [Terminal::BRIGHT_BLUE] = Widget::THEME_ANSI_BRIGHT_BLUE,
    [Terminal::BRIGHT_MAGENTA] = Widget::THEME_ANSI_BRIGHT_MAGENTA,
    [Terminal::BRIGHT_CYAN] = Widget::THEME_ANSI_BRIGHT_CYAN,
    [Terminal::BRIGHT_GREY] = Widget::THEME_ANSI_BRIGHT_WHITE,
    [Terminal::FOREGROUND] = Widget::THEME_ANSI_FOREGROUND,
    [Terminal::BACKGROUND] = Widget::THEME_ANSI_BACKGROUND,
};

Graphic::Color TerminalView::cell_color(Terminal::Color terminal_color)
{
    return theme_get_color(_color_to_role[terminal_color]);
}

TerminalView::TerminalView()
{
    _terminal = own<Terminal::Terminal>(80, 24);

    _terminal_device = IO::Terminal::create().unwrap();

    _server_notifier = own<Async::Notifier>(_terminal_device.server, POLL_READ, [this]() {
        handle_read();
    });

    _cursor_blink_timer = own<Async::Timer>(250, [this]() {
        blink();

        int cx = _terminal->cursor().x;
        int cy = _terminal->cursor().y;

        should_repaint(cell_bound(cx, cy).offset(bound().position() - Math::Vec2i{0, _scroll_offset}));
    });

    _cursor_blink_timer->start();

    Launchpad *shell_launchpad = launchpad_create("shell", "/Applications/shell/shell");
    launchpad_handle(shell_launchpad, _terminal_device.client, 0);
    launchpad_handle(shell_launchpad, _terminal_device.client, 1);
    launchpad_handle(shell_launchpad, _terminal_device.client, 2);
    launchpad_launch(shell_launchpad, nullptr);

    font(Graphic::Font::get("mono").unwrap());
}

void TerminalView::paint_cell(
    Graphic::Painter &painter,
    int x,
    int y,
    Text::Rune rune,
    Terminal::Color foreground,
    Terminal::Color background,
    Terminal::Attributes attributes)
{
    Math::Recti bound = cell_bound(x, y);

    if (attributes.invert)
    {
        std::swap(foreground, background);
    }

    if (background != Terminal::BACKGROUND)
    {
        painter.clear(bound, cell_color(background));
    }

    if (attributes.underline)
    {
        painter.draw_line(
            bound.position() + Math::Vec2i(0, 14),
            bound.position() + Math::Vec2i(bound.width(), 14),
            cell_color(foreground));
    }

    if (rune == U' ')
    {
        return;
    }

    auto &glyph = font()->glyph(rune);

    painter.draw_glyph(
        *font(),
        glyph,
        bound.position() + Math::Vec2i(0, 13),
        cell_color(foreground));

    if (attributes.bold)
    {
        painter.draw_glyph(
            *font(),
            glyph,
            bound.position() + Math::Vec2i(1, 13),
            cell_color(foreground));
    }
}

void TerminalView::paint(Graphic::Painter &painter, const Math::Recti &dirty)
{
    painter.push();
    painter.transform({0, -_scroll_offset});

    for (int y = 0; y < _terminal->height(); y++)
    {
        for (int x = 0; x < _terminal->width(); x++)
        {
            Terminal::Cell cell = _terminal->surface().at(x, y + _scroll_offset / cell_size().y());
            paint_cell(painter, x, y + _scroll_offset / cell_size().y(), cell);
            _terminal->surface().undirty(x, y);
        }
    }

    int cx = _terminal->cursor().x;
    int cy = _terminal->cursor().y - _scroll_offset / cell_size().y();

    if (cell_bound(cx, cy).colide_with(dirty))
    {
        Terminal::Cell cell = _terminal->surface().at(cx, cy);

        if (window()->focused())
        {
            if (_cursor_blink)
            {
                paint_cell(
                    painter,
                    cx,
                    cy + _scroll_offset / cell_size().y(),
                    cell.rune,
                    Terminal::BACKGROUND,
                    Terminal::FOREGROUND,
                    {});
            }
            else
            {
                paint_cell(painter, cx, cy + _scroll_offset / cell_size().y(), cell);
            }
        }
        else
        {
            paint_cell(painter, cx, cy, cell);
            painter.draw_rectangle(cell_bound(cx, cy), color(Widget::THEME_ANSI_CURSOR));
        }
    }

    painter.pop();
}

void TerminalView::event(Widget::Event *event)
{
    auto send_sequence = [&](auto sequence) {
        _terminal_device.server.write(sequence, strlen(sequence));
        event->accepted = true;
    };

    if (event->type == Widget::Event::MOUSE_SCROLL)
    {
        _scroll_offset += 48 * event->mouse.scroll;
        _scroll_offset = clamp(_scroll_offset, -_terminal->surface().scrollback() * cell_size().y(), 0);
        should_repaint();
    }
    else if (event->type == Widget::Event::MOUSE_BUTTON_PRESS)
    {
        focus();
    }
    else if (event->type == Widget::Event::KEYBOARD_KEY_TYPED)
    {
        switch (event->keyboard.key)
        {
        case KEYBOARD_KEY_DELETE:
            send_sequence("\e[3~");
            break;

        case KEYBOARD_KEY_END:
            send_sequence("\e[4~");
            break;

        case KEYBOARD_KEY_F1:
            send_sequence("\e[11~");
            break;

        case KEYBOARD_KEY_F2:
            send_sequence("\e[12~");
            break;

        case KEYBOARD_KEY_F3:
            send_sequence("\e[13~");
            break;

        case KEYBOARD_KEY_F4:
            send_sequence("\e[14~");
            break;

        case KEYBOARD_KEY_F5:
            send_sequence("\e[15~");
            break;

        case KEYBOARD_KEY_F6:
            send_sequence("\e[17~");
            break;

        case KEYBOARD_KEY_F7:
            send_sequence("\e[18~");
            break;

        case KEYBOARD_KEY_F8:
            send_sequence("\e[19~");
            break;

        case KEYBOARD_KEY_F9:
            send_sequence("\e[20~");
            break;

        case KEYBOARD_KEY_F10:
            send_sequence("\e[21~");
            break;

        case KEYBOARD_KEY_F11:
            send_sequence("\e[23~");
            break;

        case KEYBOARD_KEY_F12:
            send_sequence("\e[24~");
            break;

        case KEYBOARD_KEY_HOME:
            send_sequence("\e[1~");
            break;

        case KEYBOARD_KEY_INSERT:
            send_sequence("\e[2~");
            break;

        case KEYBOARD_KEY_PGUP:
            send_sequence("\e[5~");
            break;

        case KEYBOARD_KEY_PGDOWN:
            send_sequence("\e[6~");
            break;

        case KEYBOARD_KEY_UP:
            send_sequence("\e[A");
            break;

        case KEYBOARD_KEY_DOWN:
            send_sequence("\e[B");
            break;

        case KEYBOARD_KEY_RIGHT:
            send_sequence("\e[C");
            break;

        case KEYBOARD_KEY_LEFT:
            send_sequence("\e[D");
            break;

        default:
            if (event->keyboard.rune != 0)
            {
                uint8_t buffer[5];
                int size = Text::rune_to_utf8(event->keyboard.rune, buffer);
                _terminal_device.server.write(buffer, size);
                event->accepted = true;
            }
            break;
        }
    }
}

void TerminalView::layout()
{
    int width = bound().width() / cell_size().x();
    int height = bound().height() / cell_size().y();

    width = MAX(width, 8);
    height = MAX(height, 8);

    IO::logln("Resize {}x{}", width, height);

    if (_terminal->width() != width || _terminal->height() != height)
    {
        _terminal->resize(width, height);

        IOCallTerminalSizeArgs args = {width, height};
        _terminal_device.server.call(IOCALL_TERMINAL_SET_SIZE, &args);
    }
}

void TerminalView::handle_read()
{
    char buffer[TERMINAL_IO_BUFFER_SIZE];
    auto read_result = _terminal_device.server.read(buffer, TERMINAL_IO_BUFFER_SIZE);

    if (!read_result.success())
    {
        IO::logln("Terminal: Read from server failed: %s", read_result.description());
        return;
    }

    _terminal->write(buffer, read_result.unwrap());
    should_repaint();
}