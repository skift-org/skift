#include <libwidget/Button.h>

#include <libwidget/TitleBar.h>

#include "settings/pages/Home.h"
#include "settings/windows/MainWindow.h"

namespace Settings
{

MainWindow::MainWindow() : Window(WINDOW_RESIZABLE)
{
    size({700, 500});

    root()->layout(VFLOW(0));

    root()->add<Widget::TitleBar>(
        Graphic::Icon::get("cog"),
        "Settings");

    auto navigation_bar = root()->add(Widget::panel());
    navigation_bar->layout(HFLOW(4));
    navigation_bar->insets(4);

    navigation_bar->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("arrow-left"));
    navigation_bar->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("arrow-right"));
    navigation_bar->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("home"));

    root()->add<HomePage>();
}

} // namespace Settings
