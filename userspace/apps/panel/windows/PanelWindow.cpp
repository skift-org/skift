#include <libsystem/process/Process.h>

#include <libwidget/Elements.h>
#include <libwidget/Screen.h>

#include "panel/widgets/DateAndTime.h"
#include "panel/widgets/RessourceMonitor.h"
#include "panel/widgets/UserAccount.h"
#include "panel/windows/PanelWindow.h"

namespace panel
{

PanelWindow::PanelWindow()
    : Widget::Window(WINDOW_BORDERLESS | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC | WINDOW_NO_ROUNDED_CORNERS)
{
    type(WINDOW_TYPE_PANEL);
    opacity(0.85);

    _menu = own<MenuWindow>();
    _datetime = own<DateAndTimeWindow>();
    _quicksetting = own<QuickSettingsWindow>();

    auto container = root()->add<Widget::Element>();
    container->flags(Widget::Element::FILL);
    container->insets(Insetsi(4));

    root()->add(Widget::separator());
    root()->add(Widget::separator())->color(Widget::THEME_BORDER, Graphic::Colors::BLACK.with_alpha(0.25));

    auto menu = container->add(Widget::basic_button(Graphic::Icon::get("menu"), "Applications", [this] {
        _menu->show();
    }));

    container->add(Widget::spacer());

    auto date_and_time = container->add<DateAndTime>();

    date_and_time->on(Widget::Event::ACTION, [this](auto) {
        _datetime->show();
    });

    container->add(Widget::spacer());

    container->add<UserAccount>();

    auto ressource_monitor = container->add<RessourceMonitor>();

    ressource_monitor->on(Widget::Event::ACTION, [](auto) {
        process_run("task-manager", nullptr, 0);
    });

    auto dots = container->add(Widget::basic_button(Graphic::Icon::get("dots"), [this] {
        _quicksetting->show();
    }));

    bound(Widget::Screen::bound().take_top(root()->compute_size().y()));
    on(Widget::Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Widget::Screen::bound().take_top(root()->compute_size().y()));
    });
}

} // namespace panel