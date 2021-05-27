#pragma once

#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>

namespace Settings
{

RefPtr<Widget::Element> link(
    RefPtr<Widget::Element> icon,
    String text,
    Func<void(void)> on_click = nullptr)
{
    using namespace Widget;

    // clang-format off

    return fill(
        basic_button(
            spacing({8, 24},
                vflow(4, {
                    fill(icon),
                    label(text,Math::Anchor::CENTER)
                })
            ),
            on_click
        )
    );

    // clang-format on
}

} // namespace Settings
