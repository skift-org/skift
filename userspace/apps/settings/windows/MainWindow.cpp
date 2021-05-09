#include <libwidget/Components.h>

#include "settings/pages/Home.h"
#include "settings/windows/MainWindow.h"

namespace Settings
{

MainWindow::MainWindow() : Window(WINDOW_RESIZABLE)
{
    size({700, 500});

    root()->add(Widget::titlebar(Graphic::Icon::get("cog"), "Settings"));

    auto navigation_bar = root()->add(Widget::panel());

    navigation_bar->add(Widget::basic_button(Graphic::Icon::get("arrow-left")));
    navigation_bar->add(Widget::basic_button(Graphic::Icon::get("arrow-right")));
    navigation_bar->add(Widget::basic_button(Graphic::Icon::get("home")));

    root()->add<HomePage>();
}

} // namespace Settings
