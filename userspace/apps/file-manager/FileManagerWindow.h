#pragma once

#include <libwidget/Components.h>
#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

struct FileManagerWindow : public Widget::Window
{
    FileManagerWindow() : Window{WINDOW_RESIZABLE} {}

    RefPtr<Widget::Element> build()
    {
        return Widget::vflow({
            Widget::titlebar(Graphic::Icon::get("folder"), "File Manager"),
            Widget::toolbar({}),
        });
    }
};