#pragma once

#include "media.h"
#include "types.h"

namespace Web::Media {

template <typename T>
struct RangeBound {
    enum Type : u8 {
        NONE,
        INCLUSIVE,
        EXCLUSIVE,
    };

    T value = {};
    Type type = NONE;
};

template <typename T, auto Media::*F>
struct RangeFeature {
    using Bound = RangeBound<T>;

    Bound lower{};
    Bound upper{};

    bool match(T actual) const {
        bool result = true;

        if (lower.type == Bound::INCLUSIVE) {
            result &= actual >= lower.value;
        } else if (lower.type == Bound::EXCLUSIVE) {
            result &= actual > lower.value;
        }

        if (upper.type == Bound::INCLUSIVE) {
            result &= actual <= upper.value;
        } else if (upper.type == Bound::EXCLUSIVE) {
            result &= actual < upper.value;
        }

        return result;
    }

    bool match(Media const &media) const {
        return match(media.*F);
    }

    static RangeFeature min(T value) {
        return {
            .lower = {value, Bound::INCLUSIVE},
        };
    }

    static RangeFeature max(T value) {
        return {
            .upper = {value, Bound::INCLUSIVE},
        };
    }

    static RangeFeature exact(T value) {
        return {
            .lower = {value, Bound::INCLUSIVE},
            .upper = {value, Bound::INCLUSIVE},
        };
    }
};

template <typename T, auto Media::*F>
struct DiscreteFeature {
    T value;

    bool match(T actual) const {
        return actual == value;
    }

    bool match(Media const &media) const {
        return match(media.*F);
    }
};

/// 2.3. Media Types
/// https://drafts.csswg.org/mediaqueries/#media-types
using TypeFeature = DiscreteFeature<Type, &Media::type>;

// 4. MARK: Viewport/Page Dimensions Media Features

/// 4.1. Width: the width feature
/// https://drafts.csswg.org/mediaqueries/#width
using WidthFeature = RangeFeature<Types::Length, &Media::width>;

/// 4.2. Height: the height feature
/// https://drafts.csswg.org/mediaqueries/#height
using HeightFeature = RangeFeature<Types::Length, &Media::height>;

/// 4.3. Device Width: the device-width feature
/// https://drafts.csswg.org/mediaqueries/#aspect-ratio
using AspectRatioFeature = RangeFeature<f64, &Media::aspectRatio>;

/// 4.4. Device Height: the device-height feature
/// https://drafts.csswg.org/mediaqueries/#orientation
using OrientationFeature = DiscreteFeature<Orientation, &Media::orientation>;

// 5. MARK: Display Quality Media Features

/// 5.1. Resolution: the resolution feature
/// https://drafts.csswg.org/mediaqueries/#resolution
using ResolutionFeature = RangeFeature<Types::Resolution, &Media::resolution>;

/// 5.2. Scan: the scan feature
/// https://drafts.csswg.org/mediaqueries/#scan
using ScanFeature = DiscreteFeature<Scan, &Media::scan>;

/// 5.3. Grid: the grid feature
/// https://drafts.csswg.org/mediaqueries/#grid
using GridFeature = DiscreteFeature<bool, &Media::grid>;

/// 5.4. Update: the update feature
/// https://drafts.csswg.org/mediaqueries/#update
using UpdateFeature = DiscreteFeature<Update, &Media::update>;

/// 5.5. Overflow Block: the overflow-block feature
/// https://drafts.csswg.org/mediaqueries/#overflow-block
using OverflowBlockFeature = DiscreteFeature<OverflowBlock, &Media::overflowBlock>;

/// 5.6. Overflow Inline: the overflow-inline feature
/// https://drafts.csswg.org/mediaqueries/#overflow-inline
using OverflowInlineFeature = DiscreteFeature<OverflowInline, &Media::overflowInline>;

//  6. MARK: Color Media Features

// 6.1. Color: the color feature
/// https://drafts.csswg.org/mediaqueries/#color
using ColorFeature = RangeFeature<int, &Media::color>;

/// 6.2. Color Index: the color-index feature
/// https://drafts.csswg.org/mediaqueries/#color-index
using ColorIndexFeature = RangeFeature<int, &Media::colorIndex>;

/// 6.3. Monochrome: the monochrome feature
/// https://drafts.csswg.org/mediaqueries/#monochrome
using MonochromeFeature = RangeFeature<int, &Media::monochrome>;

/// 6.4. Color Gamut: the color-gamut feature
/// https://drafts.csswg.org/mediaqueries/#color-gamut
using ColorGamutFeature = DiscreteFeature<ColorGamut, &Media::colorGamut>;

// 7. MARK: Interaction Media Features

/// 7.1. Pointer: the pointer feature
/// https://drafts.csswg.org/mediaqueries/#pointer
using PointerFeature = DiscreteFeature<Pointer, &Media::pointer>;

/// 7.2. Hover: the hover feature
/// https://drafts.csswg.org/mediaqueries/#hover
using HoverFeature = DiscreteFeature<Hover, &Media::hover>;

/// 7.3. Any Pointer: the any-pointer feature
/// https://drafts.csswg.org/mediaqueries/#any-pointer
using AnyPointerFeature = DiscreteFeature<Pointer, &Media::anyPointer>;

/// 7.4. Any Hover: the any-hover feature
/// https://drafts.csswg.org/mediaqueries/#any-hover
using AnyHoverFeature = DiscreteFeature<Hover, &Media::anyHover>;

using _Feature = Union<
    TypeFeature,
    WidthFeature,
    HeightFeature,
    AspectRatioFeature,
    OrientationFeature,
    ResolutionFeature,
    ScanFeature,
    GridFeature,
    UpdateFeature,
    OverflowBlockFeature,
    OverflowInlineFeature,
    ColorFeature,
    ColorIndexFeature,
    MonochromeFeature,
    ColorGamutFeature,
    PointerFeature,
    HoverFeature,
    AnyPointerFeature,
    AnyHoverFeature>;

struct Feature : public _Feature {
    using _Feature::_Feature;

    bool match(Media const &media) const {
        return visit([&](auto const &feature) {
            return feature.match(media);
        });
    }

    static Feature type(Type value) {
        return TypeFeature{value};
    }

    static Feature width(RangeFeature<Types::Length, &Media::width> value) {
        return value;
    }
};

} // namespace Web::Media
