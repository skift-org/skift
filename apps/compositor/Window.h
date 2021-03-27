#pragma once

#include <libgraphic/Bitmap.h>
#include <libtest/AssertTrue.h>
#include <libutils/Rect.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>

#include "compositor/Protocol.h"

struct Client;

struct Window
{
private:
    int _id;
    WindowFlag _flags;
    WindowType _type;

    struct Client *_client;
    Recti _bound;
    Widget::CursorState _cursor_state{};

    RefPtr<Graphic::Bitmap> _frontbuffer;
    RefPtr<Graphic::Bitmap> _backbuffer;

public:
    int id() { return _id; }
    WindowFlag flags() { return _flags; };
    WindowType type() { return _type; };

    Client *client() { return _client; }

    Widget::CursorState cursor_state() { return _cursor_state; }

    void cursor_state(Widget::CursorState cursor_state) { _cursor_state = cursor_state; }

    Graphic::Bitmap &frontbuffer()
    {
        assert_true(_frontbuffer);
        return *_frontbuffer;
    }

    Window(
        int id,
        WindowFlag flags,
        WindowType type,
        struct Client *client,
        Recti bound,
        RefPtr<Graphic::Bitmap> frontbuffer,
        RefPtr<Graphic::Bitmap> backbuffer);

    ~Window();

    Recti bound();

    Recti cursor_capture_bound();

    void move(Vec2i new_position);

    void resize(Recti new_bound);

    void send_event(Widget::Event event);

    void handle_mouse_move(Vec2i old_position, Vec2i position, Widget::MouseButton buttons);

    void handle_mouse_buttons(Widget::MouseButton old_buttons, Widget::MouseButton buttons, Vec2i position);

    void handle_double_click(Vec2i position);

    void handle_mouse_scroll(Vec2i position, int scroll);

    void get_focus();

    void lost_focus();

    void flip_buffers(int frontbuffer_handle, Vec2i frontbuffer_size, int backbuffer_handle, Vec2i backbuffer_size, Recti region);
};
