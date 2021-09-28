#include <libgraphic/Painter.h>

#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>
#include <libwidget/components/TitleBarComponent.h>

namespace Widget
{

TitleBarComponent::TitleBarComponent(RefPtr<Graphic::Icon> icon, String title)
    : _icon{icon}, _title{title}
{
}

void TitleBarComponent::event(Event *event)
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
        else if (_is_dragging &&
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

RefPtr<Element> TitleBarComponent::build()
{
    bool window_resizable = window()->flags() & WINDOW_RESIZABLE;

    // clang-format off

    return spacing(
        4,
        hflow(4, {
            basic_button(_icon, _title),
            spacer(),
            window_resizable
                ? basic_button(Graphic::Icon::get("plus"), [this] {
                    window()->toggle_maximise();
                })
                : nullptr,
            basic_button(Graphic::Icon::get("close"), [this] {
                window()->try_hide();
            })
        })
    );

    // clang-format on
}

} // namespace Widget
