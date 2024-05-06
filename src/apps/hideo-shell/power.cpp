#include <hideo-base/dialogs.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>

namespace Hideo::Shell {

Ui::Child powerDialog() {
    auto actions =
        Ui::vflow(
            4,
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::LOGOUT, "Logout"),
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::SLEEP, "Sleep"),
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::RESTART, "Restart"),
            Ui::button(Ui::closeDialog, Ui::ButtonStyle::subtle(), Mdi::POWER_STANDBY, "Shutdown")
        ) |
        Ui::grow();

    return dialogScafold(
        Math::Align::CENTER | Math::Align::CLAMP,
        dialogTitle("Power Options"s) | Ui::minSize({320, Ui::UNCONSTRAINED}),
        {actions}
    );
}

} // namespace Hideo::Shell
