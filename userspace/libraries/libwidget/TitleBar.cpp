#include <libgraphic/Painter.h>

#include <libwidget/Button.h>
#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

namespace Widget
{

TitleBar::TitleBar(RefPtr<Graphic::Icon> icon, String title)
    : _icon{icon}, _title{title}
{
}

void TitleBar::event(Event *event)
{
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

RefPtr<Element> TitleBar::build()
{
    bool window_resizable = window()->flags() & WINDOW_RESIZABLE;

    // clang-format off

    return spacing(
        4,
        hflow(4, {
            button(Button::TEXT, _icon, _title),
            spacer(),
            window_resizable
                ? button(Button::TEXT, Graphic::Icon::get("plus"), [this] {
                    window()->toggle_maximise();
                })
                : nullptr,
            button(Button::TEXT, Graphic::Icon::get("close"), [this] {
                window()->hide();
            })
        })
    );

    // clang-format on
}

} // namespace Widget
