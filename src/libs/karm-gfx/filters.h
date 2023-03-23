#pragma once

#include <karm-base/box.h>
#include <karm-base/vec.h>

#include "buffer.h"
#include "color.h"

namespace Karm::Gfx {

struct Unfiltered {
    void apply(Surface) const {}
};

struct BlurFilter {
    isize amount;
    void apply(Surface) const;
};

struct SaturationFilter {
    f64 amount;
    void apply(Surface) const;
};

struct GrayscaleFilter {
    f64 amount;
    void apply(Surface) const;
};

struct ContrastFilter {
    f64 amount;
    void apply(Surface) const;
};

struct BrightnessFilter {
    f64 amount;
    void apply(Surface) const;
};

struct NoiseFilter {
    f64 amount;
    void apply(Surface) const;
};

struct SepiaFilter {
    f64 amount;
    void apply(Surface) const;
};

struct TintFilter {
    Color tint;
    void apply(Surface) const;
};

struct OverlayFilter {
    Color color;
    void apply(Surface) const;
};

struct Filter;

struct FilterChain {
    Cons<Box<Filter>> filters;
    void apply(Surface) const;
};

using _Filters = Var<
    Unfiltered,
    BlurFilter,
    SaturationFilter,
    GrayscaleFilter,
    ContrastFilter,
    BrightnessFilter,
    NoiseFilter,
    SepiaFilter,
    TintFilter,
    OverlayFilter>;

struct Filter : public _Filters {
    using _Filters::_Filters;
    void apply(Surface s) const {
        visit([&](auto const &filter) {
            filter.apply(s);
        });
    }
};

} // namespace Karm::Gfx
