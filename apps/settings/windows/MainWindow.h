#pragma once

#include <libwidget/Widgets.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/TitleBar.h>

#include "settings/pages/Home.h"

namespace settings
{

class MainWindow : public Window
{
private:
public:
    MainWindow() : Window(WINDOW_RESIZABLE)
    {
        title("Settings");
        icon(Icon::get("cog"));
        size({700, 500});

        root()->layout(VFLOW(0));

        new TitleBar(root());

        auto navigation_bar = new Panel(root());
        navigation_bar->layout(HFLOW(4));
        navigation_bar->insets(4);
        navigation_bar->max_height(38);
        navigation_bar->min_height(38);

        new Button(navigation_bar, BUTTON_TEXT, Icon::get("arrow-left"));
        new Button(navigation_bar, BUTTON_TEXT, Icon::get("arrow-right"));
        new Button(navigation_bar, BUTTON_TEXT, Icon::get("home"));

        new HomePage(root());
    }

    ~MainWindow()
    {
    }
};

} // namespace settings
