#include <skift/Environment.h>

#include <libwidget/Elements.h>
#include <libwidget/Screen.h>

#include "panel/widgets/SettingToggle.h"
#include "panel/windows/PanelWindow.h"
#include "panel/windows/QuickSettingsWindow.h"

using namespace Widget;

namespace panel
{

QuickSettingsWindow::QuickSettingsWindow()
    : Window(WINDOW_AUTO_CLOSE | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC)
{
    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);

    on(Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Screen::bound()
                  .take_right(WIDTH)
                  .shrinked({PanelWindow::HEIGHT, 0, 0, 0})
                  .with_height(root()->compute_size().y()));
    });

    bound(Screen::bound()
              .take_right(WIDTH)
              .shrinked({PanelWindow::HEIGHT, 0, 0, 0})
              .with_height(root()->compute_size().y()));
}

RefPtr<Element> QuickSettingsWindow::build()
{
    // clang-format off

    return vflow(4, {
        spacing(12,
            label("Quick settings")
        ),
        setting_toggle(Graphic::Icon::get("duck"), "Show Wireframe", "appearance:widgets.wireframe"),
        setting_toggle(Graphic::Icon::get("moon-waning-crescent"), "Night Light", "appearance:night-light.enable"),
        basic_button("Light Theme", [] {
            Settings::Service::the()->write(Settings::Path::parse("appearance:widgets.theme"), "skift-light");
        }),
        basic_button("Dark Theme", [] {
            Settings::Service::the()->write(Settings::Path::parse("appearance:widgets.theme"), "skift-dark");
        }),
        Widget::panel(
            spacing(4,
                hflow(6, {
                    basic_button(Graphic::Icon::get("account"), environment().get("POSIX").get("LOGNAME").as_string()),
                    spacer(),
                    basic_button(Graphic::Icon::get("folder"), [&] {
                        process_run("file-manager", nullptr, 0);
                    }),
                    basic_button(Graphic::Icon::get("cog"), [&] {
                        process_run("settings", nullptr, 0);
                    }),
                    basic_button(Graphic::Icon::get("power-standby"), [&] {
                        process_run("logout", nullptr, 0);
                    })
                })
            )
        )
    });

    // clang-format on
}

} // namespace panel