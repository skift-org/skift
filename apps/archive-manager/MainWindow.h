#pragma once

#include <libwidget/Container.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Browser.h>
#include <libfilepicker/widgets/JumpList.h>
#include <libfilepicker/widgets/ToolBar.h>

class MainWindow :
    public Window
{
private:
public:
    MainWindow()
        : Window(WINDOW_RESIZABLE)
    {
        icon(Icon::get("folder"));
        title("Archive Manager");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        new TitleBar(root());

        auto browser = new Container(root());

        browser->flags(Widget::FILL);
        browser->layout(HFLOW(1));

    }
};
