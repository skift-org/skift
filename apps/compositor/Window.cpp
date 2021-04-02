#include <libsystem/Logger.h>

#include "compositor/Client.h"
#include "compositor/Manager.h"
#include "compositor/Protocol.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

Window::Window(
    int id,
    WindowFlag flags,
    WindowType type,
    struct Client *client,
    Recti bound,
    RefPtr<Graphic::Bitmap> frontbuffer,
    RefPtr<Graphic::Bitmap> backbuffer)
    : _id(id),
      _flags(flags),
      _type(type),
      _client(client),
      _bound(bound),
      _frontbuffer(frontbuffer),
      _backbuffer(backbuffer)
{
    manager_register_window(this);
}

Window::~Window()
{
    manager_unregister_window(this);
}

Recti Window::bound()
{
    return _bound;
}

Recti Window::cursor_capture_bound()
{
    if (_flags & WINDOW_RESIZABLE)
    {
        return bound().expended({16});
    }
    else
    {
        return bound();
    }
}

void Window::move(Vec2i new_position)
{
    renderer_region_dirty(bound());

    _bound = _bound.moved(new_position);

    renderer_region_dirty(bound());
}

void Window::resize(Recti new_bound)
{

    if (_bound != new_bound)
    {
        renderer_region_dirty(bound());
        _bound = new_bound;
        renderer_region_dirty(bound());
    }
}

void Window::send_event(Widget::Event event)
{
    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_EVENT_WINDOW,
        .event_window = {
            .id = _id,
            .event = event,
        },
    };

    _client->send_message(message);
}

void Window::handle_mouse_move(Vec2i old_position, Vec2i position, Widget::MouseButton buttons)
{
    Widget::Event event = {
        .type = Widget::Event::MOUSE_MOVE,
        .accepted = false,
        .mouse = {
            .scroll = 0,
            .position = position,
            .old_position = old_position,
            .position_on_screen = position,
            .old_position_on_screen = old_position,
            .button = MOUSE_NO_BUTTON,
            .buttons = buttons,
        },

        .keyboard = {},
    };

    send_event(event);
}

static void handle_mouse_button(Window &window, Widget::MouseButton button, Widget::MouseButton old_buttons, Widget::MouseButton buttons, Vec2i position)
{
    bool was_button_pressed = old_buttons & button;
    bool is_button_pressed = buttons & button;

    if (is_button_pressed && !was_button_pressed)
    {
        Widget::Event event = {
            .type = Widget::Event::MOUSE_BUTTON_PRESS,
            .accepted = false,
            .mouse = {
                .scroll = 0,
                .position = position,
                .old_position = position,
                .position_on_screen = position,
                .old_position_on_screen = position,
                .button = button,
                .buttons = buttons,
            },

            .keyboard = {},
        };

        window.send_event(event);
    }

    if (was_button_pressed && !is_button_pressed)
    {
        Widget::Event event = {
            .type = Widget::Event::MOUSE_BUTTON_RELEASE,
            .accepted = false,
            .mouse = {
                .scroll = 0,
                .position = position,
                .old_position = position,
                .position_on_screen = position,
                .old_position_on_screen = position,
                .button = button,
                .buttons = buttons,
            },

            .keyboard = {},
        };

        window.send_event(event);
    }
}

void Window::handle_mouse_buttons(Widget::MouseButton old_buttons, Widget::MouseButton buttons, Vec2i position)
{
    handle_mouse_button(*this, MOUSE_BUTTON_LEFT, old_buttons, buttons, position);
    handle_mouse_button(*this, MOUSE_BUTTON_RIGHT, old_buttons, buttons, position);
    handle_mouse_button(*this, MOUSE_BUTTON_MIDDLE, old_buttons, buttons, position);
}

void Window::handle_double_click(Vec2i position)
{
    Widget::Event event = {
        .type = Widget::Event::MOUSE_DOUBLE_CLICK,
        .accepted = false,
        .mouse = {
            .scroll = 0,
            .position = position,
            .old_position = position,
            .position_on_screen = position,
            .old_position_on_screen = position,
            .button = MOUSE_BUTTON_LEFT,
            .buttons = MOUSE_BUTTON_LEFT,
        },

        .keyboard = {},
    };

    send_event(event);
}

void Window::handle_mouse_scroll(Vec2i position, int scroll)
{
    Widget::Event event = {
        .type = Widget::Event::MOUSE_SCROLL,
        .accepted = false,
        .mouse = {
            .scroll = scroll,
            .position = position,
            .old_position = position,
            .position_on_screen = position,
            .old_position_on_screen = position,
            .button = MOUSE_NO_BUTTON,
            .buttons = MOUSE_NO_BUTTON,
        },

        .keyboard = {},
    };

    send_event(event);
}

void Window::get_focus()
{
    renderer_region_dirty(bound());

    Widget::Event event = {};
    event.type = Widget::Event::GOT_FOCUS;
    send_event(event);
}

void Window::lost_focus()
{
    renderer_region_dirty(bound());

    Widget::Event event = {};
    event.type = Widget::Event::LOST_FOCUS;
    send_event(event);
}

void Window::flip_buffers(int frontbuffer_handle, Vec2i frontbuffer_size, int backbuffer_handle, Vec2i backbuffer_size, Recti region)
{
    swap(_frontbuffer, _backbuffer);

    if (_frontbuffer->handle() != frontbuffer_handle)
    {

        auto new_frontbuffer = Graphic::Bitmap::create_shared_from_handle(frontbuffer_handle, frontbuffer_size);

        if (!new_frontbuffer.success())
        {
            logger_error("Client application gave us a jankie shared memory object id");
            return;
        }

        _frontbuffer = new_frontbuffer.unwrap();
    }

    if (_backbuffer->handle() != backbuffer_handle)
    {

        auto new_backbuffer = Graphic::Bitmap::create_shared_from_handle(backbuffer_handle, backbuffer_size);

        if (!new_backbuffer.success())
        {
            logger_error("Client application gave us a jankie shared memory object id");
            return;
        }

        _backbuffer = new_backbuffer.unwrap();
    }

    renderer_region_dirty(region.offset(bound().position()));
}
