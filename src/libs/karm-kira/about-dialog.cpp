module;

#include <karm-math/align.h>

export module Karm.Kira:aboutDialog;

import Karm.Core;
import Karm.Ui;
import :badge;
import :dialog;

namespace Karm::Kira {

static constexpr Str LICENSE = R"(This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.)";

export Ui::Child licenseDialog() {
    return dialogContent({
        dialogTitleBar("License"s),
        Ui::bodySmall(LICENSE) |
            Ui::insets(16) |
            Ui::vscroll() |
            Ui::maxSize({480, Ui::UNCONSTRAINED}) |
            Ui::grow(),
    });
}

export Ui::Child aboutDialog(String name) {
    return dialogContent({
        dialogHeader({
            Ui::hflow(6, Math::Align::CENTER, dialogTitle(name), versionBadge()),
            dialogDescription(
                "Copyright © 2018-2025 The skiftOS Developers\n"
                "\n"
                "All rights reserved."s
            ),
        }),
        dialogFooter({
            button([](auto& n) {
                showDialog(n, licenseDialog());
            },
                   Ui::ButtonStyle::regular(), "License"),
            dialogAction(Ui::SINK<>, "Ok"s),
        }),
    });
}

} // namespace Karm::Kira
