#pragma once

#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

#include "snake/widgets/BoardWidget.h"

namespace Snake
{

class MainWindow : public Window
{
private:
public:
    MainWindow()
        : Window{WINDOW_NONE}
    {
        title("Snake");

        root()->layout(VFLOW(0));
        new TitleBar(root());
        auto board = new BoardWidget(root());
        board->outsets({32});
        board->focus();

        resize_to_content();
    }

    ~MainWindow()
    {
    }
};

} // namespace Snake
