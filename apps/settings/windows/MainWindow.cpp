#include <libwidget/Button.h>
#include <libwidget/Panel.h>
#include <libwidget/TitleBar.h>

#include "settings/pages/Home.h"
#include "settings/windows/MainWindow.h"

namespace Settings
{

MainWindow::MainWindow() : Window(WINDOW_RESIZABLE)
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

    new Button(navigation_bar, Button::TEXT, Icon::get("arrow-left"));
    new Button(navigation_bar, Button::TEXT, Icon::get("arrow-right"));
    new Button(navigation_bar, Button::TEXT, Icon::get("home"));

    new HomePage(root());
}

} // namespace Settings
