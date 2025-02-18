#pragma once

#include <karm-image/picture.h>
#include <karm-ui/reducer.h>

namespace Hideo::Images {

using Hist = Array<Math::Vec3f, 64>;

static inline void computeHistogram(Hist& hist, Gfx::Pixels pixels) {
    f64 max = 0;
    hist = {};

    for (auto y = 0; y < pixels.width(); ++y) {
        for (auto x = 0; x < pixels.height(); ++x) {
            auto pixel = pixels.load({x, y});

            hist[pixel.red / 4].x += 1;
            hist[pixel.green / 4].y += 1;
            hist[pixel.blue / 4].z += 1;

            max = ::max(max, hist[pixel.red / 4].x, hist[pixel.green / 4].y, hist[pixel.blue / 4].z);
        }
    }

    for (auto& h : hist) {
        h = h / max;
    }
}

struct State {
    bool isEditor = false;
    Res<Image::Picture> image;
    Hist hist;
    Gfx::Filter filter = Gfx::Unfiltered{};

    State(Res<Image::Picture> i) : image(i) {}
};

struct Refresh {};

struct ToggleEditor {};

struct SetFilter {
    Gfx::Filter filter;
};

struct ApplyFilter {};

struct SaveImage {};

using Action = Union<
    Refresh,
    ToggleEditor,
    SetFilter,
    ApplyFilter,
    SaveImage>;

Ui::Task<Action> reduce(State&, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Images
