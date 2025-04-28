module;

#include <karm-base/slice.h>

export module Hideo.Shell:mobile;

import Karm.Ui;
import :model;
import :navbar;
import :background;
import :applications;
import :settings;
import :statusbar;

using namespace Karm;

namespace Hideo::Shell {

Ui::Child mobilePanels(State const& s) {
    return Ui::stack(
        s.activePanel == Panel::APPS
            ? appsFlyout(s)
            : Ui::empty(),
        s.activePanel == Panel::SYS
            ? sysFlyout(s)
            : Ui::empty()
    );
}

Ui::Child mobileAppHost(State const& s) {
    if (isEmpty(s.instances))
        return Ui::grow(NONE);

    return first(s.instances)->build();
}

Ui::Child mobile(State const& s) {
    return Ui::stack(
        s.instances.len() == 0
            ? background(s)
            : Ui::empty(),
        Ui::vflow(
            statusbarButton(s) | Ui::slideIn(Ui::SlideFrom::TOP),
            mobileAppHost(s) | Ui::grow(),
            navbar(s)
        )
    );
}

} // namespace Hideo::Shell
