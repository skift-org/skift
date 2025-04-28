module;

#include <karm-base/string.h>
#include <karm-gfx/canvas.h>

export module Hideo.Camera;

import Mdi;
import Karm.Ui;
import Karm.Kira;

namespace Hideo::Camera {

Ui::Child appContent() {
    auto viewport = Ui::stack(
        Ui::box({.backgroundFill = Gfx::BLUE400}, Ui::empty()),
        Ui::canvas([](Gfx::Canvas& g, Math::Vec2i size) {
            g.strokeStyle(Gfx::Stroke{
                .fill = Gfx::WHITE.withOpacity(0.5),
                .width = 1,
            });

            // Rule of third guidelines
            g.stroke(Math::Edgei{0, size.y / 3, size.x, size.y / 3}.cast<f64>());
            g.stroke(Math::Edgei{0, size.y * 2 / 3, size.x, size.y * 2 / 3}.cast<f64>());

            g.stroke(Math::Edgei{size.x / 3, 0, size.x / 3, size.y}.cast<f64>());
            g.stroke(Math::Edgei{size.x * 2 / 3, 0, size.x * 2 / 3, size.y}.cast<f64>());

            // Focus reticle
            g.stroke(Math::Ellipsef{size.cast<f64>() / 2., 32});
        })
    );

    auto topBar =
        Ui::hflow(
            Ui::button(
                Ui::SINK<>,
                Ui::ButtonStyle::regular().withForegroundFill(Gfx::WHITE).withRadii(999),
                Mdi::COG
            ),

            Ui::grow(NONE),

            Ui::button(
                Ui::SINK<>,
                Ui::ButtonStyle::regular().withForegroundFill(Gfx::WHITE).withRadii(999),
                Mdi::TUNE
            )
        ) |
        Ui::insets(24);

    auto bottomBar =
        Ui::hflow(
            Ui::button(
                Ui::SINK<>,
                Ui::ButtonStyle::regular().withRadii(999).withPadding(12),
                Ui::icon(Mdi::CAMERA_FLIP, 24)
            ) | Ui::center(),

            Ui::button(
                Ui::SINK<>,
                Ui::ButtonStyle::regular().withRadii(999).withPadding(16),
                Ui::icon(Mdi::CAMERA, 38)
            ) | Ui::center() |
                Ui::grow(),
            Ui::button(
                Ui::SINK<>,
                Ui::ButtonStyle::outline().withRadii(999).withPadding(12),
                Ui::empty(24)
            ) | Ui::center()
        ) |
        Ui::box({
            .padding = {16, 24},
            .backgroundFill = Gfx::BLACK.withOpacity(0.25),
        });

    return Ui::vflow(
        Ui::stack(
            viewport,
            Ui::vflow(topBar)
        ) | Ui::grow(),
        bottomBar
    );
}

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::CAMERA,
        .title = "Camera"s,
        .body = appContent,
    });
}

} // namespace Hideo::Camera
