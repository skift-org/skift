#include <libwidget/Elements.h>
#include <libwidget/Layouts.h>
#include <libwidget/Screen.h>

#include "panel/Constants.h"
#include "panel/widgets/DateAndTime.h"
#include "panel/widgets/RessourceMonitor.h"
#include "panel/widgets/UserAccount.h"
#include "panel/windows/DateAndTime.h"
#include "panel/windows/Menu.h"
#include "panel/windows/QuickSettings.h"
#include "panel/windows/Status.h"

using namespace Widget;

namespace Panel
{

Status::Status()
    : Window{WINDOW_BORDERLESS |
             WINDOW_ALWAYS_FOCUSED |
             WINDOW_ACRYLIC |
             WINDOW_NO_ROUNDED_CORNERS}
{
    type(WINDOW_TYPE_PANEL);
    opacity(0.85);

    _menu = own<Menu>();
    _datetime = own<DateAndTime>();
    _quicksetting = own<QuickSettings>();

    bound(Screen::bound().take_top(PANEL_WINDOW_HEIGHT));
    on(Event::DISPLAY_SIZE_CHANGED, [this](auto) { should_rebuild(); });
}

RefPtr<Element> Status::build()
{
    // clang-format off

    return min_width(
        Screen::bound().width(),
            vflow({
            spacing(4,
                hflow(4,{
                    basic_button(Graphic::Icon::get("menu"), "Applications", [this] {
                        _menu->show();
                    }),
                    spacer(),
                    basic_button(date_and_time(), [this] {
                        _datetime->show();
                    }),
                    spacer(),
                    user_account(),
                    basic_button(ressource_monitor(), [] {
                        process_run("task-manager", nullptr, 0);
                    }),
                    basic_button(Graphic::Icon::get("dots"), [this] {
                        _quicksetting->show();
                    })
                })
            ),
            separator(),
        })
    );

    // clang-format on
}

} // namespace Panel
