#pragma once

#include <libwidget/Button.h>
#include <libwidget/Elements.h>
#include <libwidget/Label.h>
#include <libwidget/Layouts.h>

namespace Settings
{

RefPtr<Widget::Element> link(
    RefPtr<Widget::Element> icon,
    String text,
    Callback<void(void)> on_click = nullptr)
{
    auto button = Widget::button(Widget::Button::TEXT, on_click);

    button->add(
        Widget::spacing(
            {8, 24},
            Widget::vflow(
                4,
                {
                    Widget::fill(icon),
                    Widget::label(text, Anchor::CENTER),
                })));

    return fill(button);
}

} // namespace Settings
