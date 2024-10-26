#pragma once

#include <karm-base/box.h>
#include <karm-base/range.h>
#include <karm-base/vec.h>

#include "buffer.h"
#include "color.h"

namespace Karm::Gfx {

struct Unfiltered {
    static constexpr auto NAME = "unfiltered";

    void apply(MutPixels) const {}
};

struct BlurFilter {
    static constexpr auto NAME = "blur";
    static constexpr frange RANGE = frange::fromStartEnd(0, 32);
    static constexpr f64 DEFAULT = 16;

    f64 amount = DEFAULT;
    void apply(MutPixels) const;
};

struct SaturationFilter {
    static constexpr auto NAME = "saturation";
    static constexpr frange RANGE = frange::fromStartEnd(0, 1);
    static constexpr f64 DEFAULT = 1;

    f64 amount = DEFAULT;
    void apply(MutPixels) const;
};

struct GrayscaleFilter {
    static constexpr auto NAME = "grayscale";
    void apply(MutPixels) const;
};

struct ContrastFilter {
    static constexpr auto NAME = "contrast";
    static constexpr frange RANGE = frange::fromStartEnd(-0.5, 0.5);
    static constexpr f64 DEFAULT = 0;

    f64 amount = DEFAULT;
    void apply(MutPixels) const;
};

struct BrightnessFilter {
    static constexpr auto NAME = "brightness";
    static constexpr frange RANGE = {0, 2};
    static constexpr f64 DEFAULT = 1;

    f64 amount = DEFAULT;
    void apply(MutPixels) const;
};

struct NoiseFilter {
    static constexpr auto NAME = "noise";
    static constexpr frange RANGE = frange::fromStartEnd(0, 1);
    static constexpr f64 DEFAULT = 0.5;

    f64 amount = DEFAULT;
    void apply(MutPixels) const;
};

struct SepiaFilter {
    static constexpr auto NAME = "sepia";
    static constexpr frange RANGE = frange::fromStartEnd(0, 1);
    static constexpr f64 DEFAULT = 0.5;

    f64 amount = DEFAULT;
    void apply(MutPixels) const;
};

struct TintFilter {
    static constexpr auto NAME = "tint";
    static constexpr Color DEFAULT = Color::fromHex(0xffffff);

    Color amount = DEFAULT;
    void apply(MutPixels) const;
};

struct OverlayFilter {
    static constexpr auto NAME = "overlay";
    static constexpr Color DEFAULT{};

    Color amount = DEFAULT;
    void apply(MutPixels) const;
};

struct Filter;

struct FilterChain {
    static constexpr auto NAME = "chain";

    Cons<Box<Filter>> filters;
    void apply(MutPixels) const;
};

using _Filters = Union<
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

    void apply(MutPixels s) const {
        visit([&](auto const &filter) {
            filter.apply(s);
        });
    }
};

} // namespace Karm::Gfx
