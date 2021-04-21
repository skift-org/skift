#include <libgraphic/Painter.h>

#include <libwidget/Button.h>
#include <libwidget/Spacer.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

namespace Widget
{

TitleBar::TitleBar(Component *parent, RefPtr<Graphic::Icon> icon, String title)
    : Component(parent), _icon{icon}, _title{title}
{
    _rebuild_invoker = own<Async::Invoker>([&] {
        clear_children();
        build();
    });

    build();
}

void TitleBar::event(Event *event)
{
    UNUSED(event);

    if (is_mouse_event(event))
    {
        if (window()->maximised())
        {
            return;
        }

        if (!_is_dragging &&
            event->type == Event::MOUSE_BUTTON_PRESS &&
            event->mouse.button == MOUSE_BUTTON_LEFT)
        {
            _is_dragging = true;
            cursor(CURSOR_MOVE);
            event->accepted = true;
        }
        else if (
            _is_dragging &&
            event->type == Event::MOUSE_BUTTON_RELEASE &&
            event->mouse.button == MOUSE_BUTTON_LEFT)
        {
            _is_dragging = false;
            cursor(CURSOR_DEFAULT);
            event->accepted = true;
        }
        else if (
            _is_dragging &&
            event->type == Event::MOUSE_MOVE)
        {
            Math::Vec2i offset = event->mouse.position_on_screen - event->mouse.old_position_on_screen;
            window()->bound(window()->bound_on_screen().offset(offset));
            event->accepted = true;
        }
    }
}

void TitleBar::build()
{
    layout(HFLOW(4));
    insets(4);

    new Button(this, Button::TEXT, _icon, _title);

    new Spacer(this);

    if (window()->flags() & WINDOW_RESIZABLE)
    {
        Component *button_maximize = new Button(this, Button::TEXT, Graphic::Icon::get("plus"));
        button_maximize->on(Event::ACTION, [this](auto) {
            window()->toggle_maximise();
        });
    }

    Component *close_button = new Button(this, Button::TEXT, Graphic::Icon::get("close"));

    close_button->on(Event::ACTION, [this](auto) {
        window()->hide();
    });
}

} // namespace Widget
