#include <libwidget/Screen.h>

#include "panel/widgets/SettingToggle.h"
#include "panel/windows/PanelWindow.h"
#include "panel/windows/QuickSettingsWindow.h"

namespace panel
{

QuickSettingsWindow::QuickSettingsWindow()
    : Window(WINDOW_AUTO_CLOSE | WINDOW_ALWAYS_FOCUSED | WINDOW_ACRYLIC)
{
    title("Panel");
    bound(Screen::bound().take_right(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}).with_height(HEIGHT));
    type(WINDOW_TYPE_POPOVER);
    opacity(0.85);

    on(Event::DISPLAY_SIZE_CHANGED, [this](auto) {
        bound(Screen::bound().take_right(WIDTH).shrinked({PanelWindow::HEIGHT, 0, 0, 0}).with_height(HEIGHT));
    });

    root()->layout(VFLOW(4));
    root()->insets(6);

    new Label(root(), "Quick settings");

    new SettingToggle(root(), "Show Wireframe", Icon::get("duck"), "appearance:widgets.wireframe");
    new SettingToggle(root(), "Night Light", Icon::get("moon-waning-crescent"), "appearance:night-light.enable");
}

} // namespace panel