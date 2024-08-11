#pragma once

#include <karm-ui/view.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child card(Ui::Child child);

Ui::Child card(Ui::Children children);

inline auto card() {
    return [](Ui::Child child) {
        return card(child);
    };
}

static inline Ui::Child card(Ui::Child child, Meta::Same<Ui::Child> auto... children) {
    return card({child, children...});
}

} // namespace Karm::Kira
