#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libsystem/process/Launchpad.h>
#include <libwidget/Event.h>
#include <libwidget/Window.h>

#include "terminal/Common.h"
#include "terminal/TerminalWidget.h"

#define TERMINAL_IO_BUFFER_SIZE 4096

void terminal_widget_server_callback(TerminalWidget *widget, Stream *server, PollEvent events)
{
    __unused(events);

    char buffer[TERMINAL_IO_BUFFER_SIZE];
    size_t size = stream_read(server, buffer, TERMINAL_IO_BUFFER_SIZE);

    if (handle_has_error(server))
    {
        handle_printf_error(server, "Terminal: read from server failed");
        return;
    }

    widget->terminal()->write(buffer, size);
    widget->should_repaint();
}

TerminalWidget::TerminalWidget(Widget *parent) : Widget(parent)
{
    _terminal = new terminal::Terminal(80, 24);

    stream_create_term(
        &_server_stream,
        &_client_stream);

    _server_notifier = notifier_create(
        this,
        HANDLE(_server_stream),
        POLL_READ,
        (NotifierCallback)terminal_widget_server_callback);

    _cursor_blink_timer = own<Timer>(250, [this]() {
        blink();

        int cx = terminal()->cursor().x;
        int cy = terminal()->cursor().y;

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
    delete _terminal;

    notifier_destroy(_server_notifier);

    stream_close(_server_stream);
    stream_close(_client_stream);
}

void TerminalWidget::paint(Painter &painter, Rectangle rectangle)
{
    painter.clear_rectangle(rectangle, color(THEME_ANSI_BACKGROUND));

    painter.push();
    painter.transform(bound().position());

    rectangle = rectangle.offset(-bound().position());

    terminal::Terminal *terminal = _terminal;

    for (int y = 0; y < terminal->height(); y++)
    {
        for (int x = 0; x < terminal->width(); x++)
        {
            terminal::Cell cell = terminal->cell_at(x, y);
            render_cell(painter, x, y, cell);
            terminal->cell_undirty(x, y);
        }
    }

    int cx = terminal->cursor().x;
    int cy = terminal->cursor().y;

    if (cell_bound(cx, cy).colide_with(rectangle))
    {
        terminal::Cell cell = terminal->cell_at(cx, cy);

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

    painter.pop();
}

void TerminalWidget::event(Event *event)
{
    auto send_sequence = [&](auto sequence) {
        stream_format(_server_stream, sequence);
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
        stream_call(_server_stream, IOCALL_TERMINAL_SET_SIZE, &args);
    }
}
