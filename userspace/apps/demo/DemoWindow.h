#pragma once

#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

#include "demo/DemoHostElement.h"

struct DemoWindow : public Widget::Window
{
    DemoWindow() : Window{WINDOW_RESIZABLE} {}

    RefPtr<Widget::Element> build()
    {
        return Widget::stateful<int>([](auto state, auto update) {
            Demo demos[] = {
                Demo{"Path", path_draw},
                Demo{"Colors", colors_draw},
                Demo{"Graphics", graphics_draw},
                Demo{"Lines", lines_draw},
            };

            Vector<RefPtr<Widget::Element>> buttons{};

            for (int i = 0; i < 4; i++)
            {
                if (i == state)
                {
                    buttons.push_back(Widget::filled_button(demos[i].name));
                }
                else
                {
                    buttons.push_back(Widget::basic_button(demos[i].name, [=] {
                        update(i);
                    }));
                }
            }

            return Widget::vflow({
                Widget::titlebar(Graphic::Icon::get("duck"), "Demos"),
                Widget::toolbar(buttons),
                demo_host(demos[state]),
            });
        });
    }
};