#pragma once

#include "model.h"

namespace Hideo::Shell {

struct MockLauncher : public Launcher {
    using Launcher::Launcher;

    void launch(State &) override;
};

struct MockInstance : public Instance {
    Mdi::Icon icon;
    String name;
    Gfx::ColorRamp ramp;

    MockInstance(Mdi::Icon icon, String name, Gfx::ColorRamp ramp)
        : icon(icon), name(name), ramp(ramp) {}

    Ui::Child build() const override;
};

} // namespace Hideo::Shell
