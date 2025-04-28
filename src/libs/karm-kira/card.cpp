module;

#include <karm-meta/traits.h>

export module Karm.Kira:card;

import Karm.Ui;

namespace Karm::Kira {

Ui::Child card(Ui::Child child) {
    return Ui::box(
        {
            .borderRadii = 4,
            .backgroundFill = Ui::GRAY900,
        },
        child
    );
}

Ui::Child card(Ui::Children children) {
    return card(Ui::vflow(children));
}

export auto card() {
    return [](Ui::Child child) {
        return card(child);
    };
}

export Ui::Child card(Ui::Child child, Meta::Same<Ui::Child> auto... children) {
    return card({child, children...});
}

} // namespace Karm::Kira
