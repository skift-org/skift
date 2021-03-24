#include <libsystem/process/Process.h>

#include <libwidget/Container.h>
#include <libwidget/Screen.h>
#include <libwidget/Separator.h>
#include <libwidget/Spacer.h>

#include "panel/widgets/DateAndTime.h"
#include "panel/widgets/RessourceMonitor.h"
#include "panel/widgets/UserAccount.h"
#include "panel/windows/PanelWindow.h"

namespace panel
{

PanelWindow::PanelWindow()
    : Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC | WINDOW_NO_ROUNDED_CORNERS)
{
    title("Panel");
    type(WINDOW_TYPE_PANEL);
    bound(Screen::bound().take_top(HEIGHT));
    opacity(0.85);
    on(Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Screen::bound().take_top(HEIGHT));
    });

    _menu = own<MenuWindow>();
    _datetime = own<DateAndTimeWindow>();
    _quicksetting = own<QuickSettingsWindow>();

    root()->layout(VFLOW(0));

    auto container = new Container(root());
    container->flags(Widget::FILL);
    container->layout(HFLOW(8));
    container->insets(Insetsi(4));

    new Separator(root());
    (new Separator(root()))->color(THEME_BORDER, Colors::BLACK.with_alpha(0.25));

    auto menu = new Button(container, Button::TEXT, Icon::get("menu"), "Applications");
    menu->on(Event::ACTION, [this](auto) {
        _menu->show();
    });

    new Spacer(container);

    auto date_and_time = new DateAndTime(container);

    date_and_time->on(Event::ACTION, [this](auto) {
        _datetime->show();
    });

    new Spacer(container);

    new UserAccount(container);

    auto ressource_monitor = new RessourceMonitor(container);

    ressource_monitor->on(Event::ACTION, [](auto) {
        process_run("task-manager", nullptr, 0);
    });

    auto dots = new Button(container, Button::TEXT, Icon::get("dots"));

    dots->on(Event::ACTION, [this](auto) {
        _quicksetting->show();
    });
}

} // namespace panel