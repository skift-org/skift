#pragma once

#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

#include "terminal/TerminalView.h"

struct TerminalWindow :
    public Widget::Window
{
    TerminalWindow() : Window(WINDOW_RESIZABLE | WINDOW_ACRYLIC)
    {
        opacity(0.85);
    }

    RefPtr<Widget::Element> build() override
    {
        return Widget::vflow({
            Widget::titlebar(Graphic::Icon::get("console-line"), "Terminal"),
            Widget::fill(Widget::spacing({0, 6, 6, 6}, terminal_view())),
        });
    }
};