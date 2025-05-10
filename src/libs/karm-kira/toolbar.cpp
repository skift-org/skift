export module Karm.Kira:toolbar;

import Karm.Ui;

import :separator;

namespace Karm::Kira {

export Ui::Child toolbar(Ui::Children children) {
    return Ui::vflow(
        Ui::hflow(4, children) |
            Ui::insets(8),
        separator()
    );
}

export Ui::Child bottombar(Ui::Children children) {
    return Ui::vflow(
        separator(),
        Ui::hflow(4, children) |
            Ui::insets(8)
    );
}

} // namespace Karm::Kira
