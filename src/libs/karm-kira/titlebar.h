#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

enum struct TitlebarStyle {
    DEFAULT,
    FIXED,
    DIALOG
};

Ui::Child titlebarTitle(Mdi::Icon icon, String title, bool compact = false);

Ui::Child titlebarControls(TitlebarStyle style = TitlebarStyle::DEFAULT);

Ui::Child titlebarContent(Ui::Children children);

Ui::Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style = TitlebarStyle::DEFAULT);

Ui::Child titlebar(Mdi::Icon icon, String title, Ui::Child middle, TitlebarStyle style = TitlebarStyle::DEFAULT);

} // namespace Karm::Kira
