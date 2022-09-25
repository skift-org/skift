#pragma once

#include <karm-ui/node.h>

namespace Karm::Ui {

enum struct TitlebarStyle {
    DEFAULT,
    FIXED,
    DIALOG,
};

Child titlebar(Media::Icons icon, String title, TitlebarStyle style = TitlebarStyle::DEFAULT);

enum struct BadgeStyle {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,
};

Child badge(BadgeStyle style, String text);

Child separator();

Child toolbar(Children children);

inline Ui::Child toolbar(Meta::Same<Child> auto... children) {
    return toolbar({children...});
}

} // namespace Karm::Ui
