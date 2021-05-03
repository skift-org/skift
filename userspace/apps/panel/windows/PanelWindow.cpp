#include <libsystem/process/Process.h>

#include <libwidget/Elements.h>
#include <libwidget/Screen.h>

#include "panel/widgets/DateAndTime.h"
#include "panel/widgets/RessourceMonitor.h"
#include "panel/widgets/UserAccount.h"
#include "panel/windows/PanelWindow.h"

using namespace Widget;

namespace panel
{

PanelWindow::PanelWindow()
    : Window{WINDOW_BORDERLESS |
             WINDOW_ALWAYS_FOCUSED |
             WINDOW_ACRYLIC |
             WINDOW_NO_ROUNDED_CORNERS}
{
    type(WINDOW_TYPE_PANEL);
    opacity(0.85);

    _menu = own<MenuWindow>();
    _datetime = own<DateAndTimeWindow>();
    _quicksetting = own<QuickSettingsWindow>();

    bound(Screen::bound().take_top(root()->compute_size().y()));
    on(Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Screen::bound().take_top(root()->compute_size().y()));
    });
}

RefPtr<Element> PanelWindow::build()
{
    // clang-format off

    return vflow({
        spacing(4,
            hflow(4,{
                basic_button(Graphic::Icon::get("menu"), "Applications", [this] {
                    _menu->show();
                }),
                spacer(),
                date_and_time([this] {
                    _datetime->show();
                }),
                spacer(),
                user_account(),
                ressource_monitor([] {
                    process_run("task-manager", nullptr, 0);
                }),
                basic_button(Graphic::Icon::get("dots"), [this] {
                    _quicksetting->show();
                })
            })
        ),
        separator(),
    });

    // clang-format on
}

} // namespace panel