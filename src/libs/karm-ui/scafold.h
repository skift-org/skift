#pragma once

#include <karm-ui/node.h>

namespace Karm::Ui {

Ui::Child titlebar(Media::Icons icon, String title);

Ui::Child separator();

Ui::Child toolbar(Children children);

static inline Ui::Child toolbar(Meta::Same<Child> auto... children) {
    return toolbar({children...});
}

} // namespace Karm::Ui
