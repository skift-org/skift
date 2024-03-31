#include <karm-ui/box.h>
#include <karm-ui/drag.h>

#include "app.h"

namespace Hideo::Shell {

Ui::Child lock(State const &state) {
    auto [date, time] = state.dateTime;
    auto dateTime = Io::format(
        // Mon. 28 Jul
        "{}. {} {}",
        date.month.abbr(),
        date.dayOfMonth() + 1,
        date.year.val()
    );

    auto clock = Ui::vflow(
        16,
        Layout::Align::HCENTER,
        Ui::displayMedium("{02}:{02}", time.hour, time.minute) | Ui::center(),
        Ui::titleMedium(dateTime.unwrap()) | Ui::center()
    );

    auto hintText = Ui::vflow(
        Ui::center(Ui::icon(Mdi::CHEVRON_UP, 48)),
        Ui::center(Ui::labelLarge(
            state.isMobile
                ? "Swipe up to unlock"
                : "Swipe up or press any key to unlock"
        ))
    );

    return Ui::stack(
        background(state),
        Ui::vflow(clock, Ui::grow(NONE), hintText | Ui::slideIn(Ui::SlideFrom::BOTTOM)) |
            Ui::spacing(state.isMobile ? 64 : 128) |
            Ui::dragRegion() |
            Ui::dismisable(Model::bind<Unlock>(), Ui::DismisDir::TOP, 0.3) |
            Ui::align(Layout::Align::VFILL | Layout::Align::HCENTER)
    );
}

} // namespace Hideo::Shell
