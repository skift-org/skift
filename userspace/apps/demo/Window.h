#pragma once

#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

#include "demo/Host.h"

namespace Demo
{

struct Window : public Widget::Window
{

    Window() : Widget::Window{WINDOW_RESIZABLE} {}

    RefPtr<Widget::Element> build()
    {
        return Widget::stateful<size_t>([](auto state, auto update) {
            static const Demo DEMOS[] = {
                {"Path", path},
                {"Colors", colors},
                {"Graphics", graphics},
                {"Lines", lines},
            };

            Vector<RefPtr<Widget::Element>> buttons{};

            for (size_t i = 0; i < ARRAY_LENGTH(DEMOS); i++)
            {
                if (i == state)
                {
                    buttons.push_back(Widget::filled_button(DEMOS[i].name));
                }
                else
                {
                    buttons.push_back(Widget::basic_button(DEMOS[i].name, [=] {
                        update(i);
                    }));
                }
            }

            return Widget::vflow({
                Widget::titlebar(Graphic::Icon::get("duck"), "Demos"),
                Widget::toolbar(buttons),
                demo_host(DEMOS[state]),
            });
        });
    }
};

} // namespace Demo