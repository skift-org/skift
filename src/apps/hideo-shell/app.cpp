module;

#include <karm-app/host.h>
#include <karm-gfx/shadow.h>
#include <karm-math/align.h>

export module Hideo.Shell:app;

import Mdi;
import Karm.Ui;
import Karm.Kira;
import Hideo.Keyboard;
import :model;
import :mobile;
import :desktop;
import :lock;

using namespace Karm;

namespace Hideo::Shell {

// MARK: Shells ----------------------------------------------------------------

export Ui::Child app(State state) {
    return Ui::reducer<Model>(
        std::move(state),
        [](auto const& state) {
            auto content =
                Ui::stack(
                    state.locked
                        ? lock(state)
                        : (state.isMobile ? mobile(state)
                                          : desktop(state)),

                    state.isMobile
                        ? mobilePanels(state)
                        : desktopPanels(state)
                ) |
                Ui::dialogLayer() |
                Ui::pinSize(
                    state.isMobile ? Math::Vec2i{411, 731}
                                   : Math::Vec2i{1280, 720}
                );

            if (state.nightLight) {
                content = Ui::foregroundFilter(Gfx::SepiaFilter{0.7}, content);
            }

            content = Ui::foregroundFilter(
                Gfx::BrightnessFilter{state.brightness},
                content
            );

            return content;
        }
    );
}

} // namespace Hideo::Shell
