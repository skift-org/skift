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
    : Widget::Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC | WINDOW_NO_ROUNDED_CORNERS)
{
    title("Panel");
    type(WINDOW_TYPE_PANEL);
    opacity(0.85);

    _menu = own<MenuWindow>();
    _datetime = own<DateAndTimeWindow>();
    _quicksetting = own<QuickSettingsWindow>();

    root()->layout(VFLOW(0));

    auto container = new Widget::Container(root());
    container->flags(Widget::Component::FILL);
    container->layout(HFLOW(8));
    container->insets(Insetsi(4));

    new Widget::Separator(root());
    (new Widget::Separator(root()))->color(Widget::THEME_BORDER, Graphic::Colors::BLACK.with_alpha(0.25));

    auto menu = new Widget::Button(container, Widget::Button::TEXT, Graphic::Icon::get("menu"), "Applications");
    menu->on(Widget::Event::ACTION, [this](auto) {
        _menu->show();
    });

    new Widget::Spacer(container);

    auto date_and_time = new DateAndTime(container);

    date_and_time->on(Widget::Event::ACTION, [this](auto) {
        _datetime->show();
    });

    new Widget::Spacer(container);

    new UserAccount(container);

    auto ressource_monitor = new RessourceMonitor(container);

    ressource_monitor->on(Widget::Event::ACTION, [](auto) {
        process_run("task-manager", nullptr, 0);
    });

    auto dots = new Widget::Button(container, Widget::Button::TEXT, Graphic::Icon::get("dots"));

    dots->on(Widget::Event::ACTION, [this](auto) {
        _quicksetting->show();
    });

    bound(Widget::Screen::bound().take_top(root()->compute_size().y()));
    on(Widget::Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Widget::Screen::bound().take_top(root()->compute_size().y()));
    });
}

} // namespace panel