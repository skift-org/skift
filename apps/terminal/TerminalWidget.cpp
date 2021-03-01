#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libsystem/process/Launchpad.h>
#include <libwidget/Event.h>
#include <libwidget/Window.h>

#include "terminal/Common.h"
#include "terminal/TerminalWidget.h"

#define TERMINAL_IO_BUFFER_SIZE 4096

TerminalWidget::TerminalWidget(Widget *parent) : Widget(parent)
{
    _terminal = own<terminal::Terminal>(80, 24);

    assert(stream_create_term(&_server_stream, &_client_stream) == SUCCESS);

    _server_notifier = own<Notifier>(HANDLE(_server_stream), POLL_READ, [this]() {
        handle_read();
    });

    _cursor_blink_timer = own<Timer>(250, [this]() {
        blink();

        int cx = _terminal->cursor().x;
        int cy = _terminal->cursor().y;

        should_repaint(cell_bound(cx, cy).offset(bound().position()));
    });

    _cursor_blink_timer->start();

    Launchpad *shell_launchpad = launchpad_create("shell", "/Applications/shell/shell");
    launchpad_handle(shell_launchpad, HANDLE(_client_stream), 0);
    launchpad_handle(shell_launchpad, HANDLE(_client_stream), 1);
    launchpad_handle(shell_launchpad, HANDLE(_client_stream), 2);
    launchpad_launch(shell_launchpad, nullptr);
}

TerminalWidget::~TerminalWidget()
{
    stream_close(_server_stream);
    stream_close(_client_stream);
}

void TerminalWidget::paint(Painter &painter, const Recti &dirty)
{
    for (int y = 0; y < _terminal->height(); y++)
    {
        for (int x = 0; x < _terminal->width(); x++)
        {
            terminal::Cell cell = _terminal->cell_at(x, y);
            render_cell(painter, x, y, cell);
            _terminal->cell_undirty(x, y);
        }
    }

    int cx = _terminal->cursor().x;
    int cy = _terminal->cursor().y;

    if (cell_bound(cx, cy).colide_with(dirty))
    {
        terminal::Cell cell = _terminal->cell_at(cx, cy);

        if (window()->focused())
        {
            if (_cursor_blink)
            {
                render_cell(
                    painter,
                    cx,
                    cy,
                    cell.codepoint,
                    terminal::BACKGROUND,
                    terminal::FOREGROUND,
                    terminal::Attributes::defaults());
            }
            else
            {
                render_cell(painter, cx, cy, cell);
            }
        }
        else
        {
            render_cell(painter, cx, cy, cell);
            painter.draw_rectangle(cell_bound(cx, cy), color(THEME_ANSI_CURSOR));
        }
    }
}

void TerminalWidget::event(Event *event)
{
    auto send_sequence = [&](auto sequence) {
        stream_write(_server_stream, sequence, strlen(sequence));
        event->accepted = true;
    };

    if (event->type == Event::KEYBOARD_KEY_TYPED)
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
            if (event->keyboard.codepoint != 0)
            {
                uint8_t buffer[5];
                int size = codepoint_to_utf8(event->keyboard.codepoint, buffer);
                stream_write(_server_stream, buffer, size);
                event->accepted = true;
            }
            break;
        }
    }
}

void TerminalWidget::do_layout()
{
    int width = bound().width() / cell_size().x();
    int height = bound().height() / cell_size().y();

    width = MAX(width, 8);
    height = MAX(height, 8);

    if (_terminal->width() != width ||
        _terminal->height() != height)
    {
        _terminal->resize(width, height);

        IOCallTerminalSizeArgs args = {width, height};
        assert(stream_call(_server_stream, IOCALL_TERMINAL_SET_SIZE, &args) == SUCCESS);
    }
}

void TerminalWidget::handle_read()
{
    char buffer[TERMINAL_IO_BUFFER_SIZE];
    size_t size = stream_read(_server_stream, buffer, TERMINAL_IO_BUFFER_SIZE);

    if (handle_has_error(_server_stream))
    {
        handle_printf_error(_server_stream, "Terminal: read from server failed");
        return;
    }

    _terminal->write(buffer, size);
    should_repaint();
}