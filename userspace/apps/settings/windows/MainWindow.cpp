#include <libwidget/Button.h>
#include <libwidget/Panel.h>
#include <libwidget/TitleBar.h>

#include "settings/pages/Home.h"
#include "settings/windows/MainWindow.h"

namespace Settings
{

MainWindow::MainWindow() : Window(WINDOW_RESIZABLE)
{
    size({700, 500});

    root()->layout(VFLOW(0));

    new Widget::TitleBar(
        root(),
        Graphic::Icon::get("cog"),
        "Settings");

    auto navigation_bar = new Widget::Panel(root());
    navigation_bar->layout(HFLOW(4));
    navigation_bar->insets(4);

    new Widget::Button(navigation_bar, Widget::Button::TEXT, Graphic::Icon::get("arrow-left"));
    new Widget::Button(navigation_bar, Widget::Button::TEXT, Graphic::Icon::get("arrow-right"));
    new Widget::Button(navigation_bar, Widget::Button::TEXT, Graphic::Icon::get("home"));

    new HomePage(root());
}

} // namespace Settings
