#include <karm-ui/box.h>
#include <karm-ui/drag.h>

#include "app.h"

namespace Shell {

Ui::Child lock(State const &state) {
    auto clock = Ui::vflow(
        Ui::center(Ui::text(Ui::TextStyle::displayMedium(), "22:07")),
        Ui::empty(16),
        Ui::center(Ui::text(Ui::TextStyle::titleMedium(), "Wed. 12 October")));

    auto hintText = Ui::vflow(
        Ui::center(Ui::icon(Mdi::CHEVRON_UP, 48)),
        Ui::center(Ui::text(
            Ui::TextStyle::labelLarge(),
            state.isMobile
                ? "Swipe up to unlock"
                : "Swipe up or press any key to unlock")));

    return Ui::stack(
        background(state),
        Ui::vflow(clock, Ui::grow(NONE), hintText | Ui::slideIn(Ui::SlideFrom::BOTTOM)) |
            Ui::spacing(state.isMobile ? 64 : 128) |
            Ui::dragRegion() |
            Ui::dismisable(Model::bind<Unlock>(), Ui::DismisDir::TOP, 0.3) |
            Ui::align(Layout::Align::VFILL | Layout::Align::HCENTER));
}

} // namespace Shell
