#pragma once

#include <libwidget/Components.h>
#include <libwidget/Window.h>

#include "libgraphic/Icon.h"
#include "snake/widgets/BoardWidget.h"

namespace Snake
{

class MainWindow : public Widget::Window
{
public:
    MainWindow()
    {
        root()->add(Widget::titlebar(Graphic::Icon::get("snake"), "Snake"));

        auto board = root()->add<BoardWidget>();
        board->outsets({32});
        board->focus();

        resize_to_content();
    }

    RefPtr<Widget::Element> build()
    {
        return Widget::vflow({
            Widget::titlebar(Graphic::Icon::get("snake"), "Snake"),
        });
    }
};

} // namespace Snake
