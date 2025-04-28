module;

#include <karm-gfx/icon.h>

export module Hideo.Shell:mock;

import Karm.Kira;
import Karm.Ui;
import :model;

namespace Hideo::Shell {

export struct MockInstance : public Instance {
    Gfx::Icon icon;
    String name;
    Gfx::ColorRamp ramp;

    MockInstance(Gfx::Icon icon, String name, Gfx::ColorRamp ramp)
        : icon(icon), name(name), ramp(ramp) {}

    Ui::Child build() const override {
        return Kr::scaffold({
                   .icon = icon,
                   .title = name,
                   .body = [name = this->name] {
                       return Ui::labelMedium(name) | Ui::center();
                   },
               }) |
               Ui::box({
                   .backgroundFill = Ui::GRAY950,
               });
    }
};

export struct MockLauncher : public Launcher {
    using Launcher::Launcher;

    void launch(State& s) override {
        auto instance = makeRc<MockInstance>(
            icon,
            name,
            ramp
        );
        s.instances.emplaceFront(instance);
    }
};

} // namespace Hideo::Shell
