#pragma once

#include <karm-ui/box.h>
#include <karm-ui/layout.h>
#include <karm-ui/node.h>

#include "model.h"

namespace Hideo::Shell {

inline auto panel(Math::Vec2i size = {500, 400}) {
    return Ui::pinSize(size) |
           Ui::box({
               .padding = 8,
               .borderRadii = 8,
               .borderWidth = 1,
               .borderPaint = Ui::GRAY800,
               .backgroundPaint = Ui::GRAY950,
           });
}

Ui::Child background(State const &state);

Ui::Child lock(State const &state);

Ui::Child appsPanel(State const &state);

Ui::Child appsFlyout(State const &state);

Ui::Child sysPanel(State const &state);

Ui::Child notiPanel(State const &state);

Ui::Child sysFlyout(State const &state);

Ui::Child powerDialog();

Ui::Child app(State state);

} // namespace Hideo::Shell
