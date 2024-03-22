#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>

namespace Hideo::Shell {

Ui::Child powerDialog() {
    return Ui::dialogScafold(
        Layout::Align::CENTER | Layout::Align::CLAMP,
        Ui::vflow(
            4,
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::LOGOUT, "Logout"),
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::SLEEP, "Sleep"),
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::RESTART, "Restart"),
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::POWER_STANDBY, "Shutdown"),
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::primary(), "Cancel")
        ) |
            Ui::spacing(6) | Ui::minSize({320, Ui::UNCONSTRAINED})
    );
}

} // namespace Hideo::Shell
