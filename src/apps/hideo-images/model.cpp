module;

#include <karm-gfx/filters.h>

export module Hideo.Images:model;

import Karm.Image;
import Karm.Ui;

using namespace Karm;

namespace Hideo::Images {

export using Hist = Array<Math::Vec3f, 64>;

void computeHistogram(Hist& hist, Gfx::Pixels pixels) {
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

export struct State {
    bool isEditor = false;
    Res<Image::Picture> image;
    Hist hist;
    Gfx::Filter filter = Gfx::Unfiltered{};

    State(Res<Image::Picture> i) : image(i) {}
};

export struct Refresh {};

export struct ToggleEditor {};

export struct SetFilter {
    Gfx::Filter filter;
};

export struct ApplyFilter {};

export struct SaveImage {};

export using Action = Union<
    Refresh,
    ToggleEditor,
    SetFilter,
    ApplyFilter,
    SaveImage>;

Ui::Task<Action> reduce(State& s, Action a) {
    a.visit(Visitor{
        [&](Refresh) {
        },
        [&](ToggleEditor) {
            s.filter = Gfx::Unfiltered{};
            s.isEditor = !s.isEditor;
        },
        [&](SetFilter f) {
            s.filter = f.filter;
        },
        [&](ApplyFilter) {
        },
        [&](SaveImage) {
        },
    });

    if (s.isEditor and s.image) {
        computeHistogram(s.hist, s.image.unwrap());
    }

    return NONE;
}

export using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Images
