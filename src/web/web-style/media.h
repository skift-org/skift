#pragma once

#include <karm-base/box.h>
#include <web-base/length.h>
#include <web-base/media.h>
#include <web-base/resolution.h>

namespace Web::Style {

// MARK: Media -----------------------------------------------------------------

struct Media {
    /// 2.3. Media Types
    /// https://drafts.csswg.org/mediaqueries/#media-types
    MediaType type;

    // 4. MARK: Viewport/Page Dimensions Media Features

    /// 4.1. Width: the width feature
    /// https://drafts.csswg.org/mediaqueries/#width
    Length width;

    /// 4.2. Height: the height feature
    /// https://drafts.csswg.org/mediaqueries/#height
    Length height;

    /// 4.3. Device Width: the device-width feature
    /// https://drafts.csswg.org/mediaqueries/#aspect-ratio
    f64 aspectRatio;

    /// 4.4. Device Height: the device-height feature
    /// https://drafts.csswg.org/mediaqueries/#orientation
    Orientation orientation;

    // 5. MARK: Display Quality Media Features

    /// 5.1. Resolution: the resolution feature
    /// https://drafts.csswg.org/mediaqueries/#resolution
    Resolution resolution;

    /// 5.2. Scan: the scan feature
    /// https://drafts.csswg.org/mediaqueries/#scan
    Scan scan;

    /// 5.3. Grid: the grid feature
    /// https://drafts.csswg.org/mediaqueries/#grid
    bool grid;

    /// 5.4. Update: the update feature
    /// https://drafts.csswg.org/mediaqueries/#update
    Update update;

    /// 5.5. Overflow Block: the overflow-block feature
    /// https://drafts.csswg.org/mediaqueries/#overflow-block
    OverflowBlock overflowBlock;

    /// 5.6. Overflow Inline: the overflow-inline feature
    /// https://drafts.csswg.org/mediaqueries/#overflow-inline
    OverflowInline overflowInline;

    //  6. MARK: Color Media Features

    // 6.1. Color: the color feature
    /// https://drafts.csswg.org/mediaqueries/#color
    int color;

    /// 6.2. Color Index: the color-index feature
    /// https://drafts.csswg.org/mediaqueries/#color-index
    int colorIndex;

    /// 6.3. Monochrome: the monochrome feature
    /// https://drafts.csswg.org/mediaqueries/#monochrome
    int monochrome;

    /// 6.4. Color Gamut: the color-gamut feature
    /// https://drafts.csswg.org/mediaqueries/#color-gamut
    ColorGamut colorGamut;

    // 7. MARK: Interaction Media Features

    /// 7.1. Pointer: the pointer feature
    /// https://drafts.csswg.org/mediaqueries/#pointer
    Pointer pointer;

    /// 7.2. Hover: the hover feature
    /// https://drafts.csswg.org/mediaqueries/#hover
    Hover hover;

    /// 7.3. Any Pointer: the any-pointer feature
    /// https://drafts.csswg.org/mediaqueries/#any-pointer
    Pointer anyPointer;

    /// 7.4. Any Hover: the any-hover feature
    /// https://drafts.csswg.org/mediaqueries/#any-hover
    Hover anyHover;
};

// MARK: Media Features --------------------------------------------------------

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
using TypeFeature = DiscreteFeature<MediaType, &Media::type>;

// 4. MARK: Viewport/Page Dimensions Media Features ----------------------------

/// 4.1. Width: the width feature
/// https://drafts.csswg.org/mediaqueries/#width
using WidthFeature = RangeFeature<Length, &Media::width>;

/// 4.2. Height: the height feature
/// https://drafts.csswg.org/mediaqueries/#height
using HeightFeature = RangeFeature<Length, &Media::height>;

/// 4.3. Device Width: the device-width feature
/// https://drafts.csswg.org/mediaqueries/#aspect-ratio
using AspectRatioFeature = RangeFeature<f64, &Media::aspectRatio>;

/// 4.4. Device Height: the device-height feature
/// https://drafts.csswg.org/mediaqueries/#orientation
using OrientationFeature = DiscreteFeature<Orientation, &Media::orientation>;

// 5. MARK: Display Quality Media Features

/// 5.1. Resolution: the resolution feature
/// https://drafts.csswg.org/mediaqueries/#resolution
using ResolutionFeature = RangeFeature<Resolution, &Media::resolution>;

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

//  6. MARK: Color Media Features ----------------------------------------------

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

    static Feature type(MediaType value) {
        return TypeFeature{value};
    }

    static Feature width(RangeFeature<Length, &Media::width> value) {
        return value;
    }
};

// MARK: Media Queries ---------------------------------------------------------

struct MediaQuery {
    struct _Infix {
        enum struct Type {
            AND,
            OR,
        };

        Type type;
        Box<MediaQuery> lhs;
        Box<MediaQuery> rhs;

        bool match(Media const &media) const {
            switch (type) {
            case Type::AND:
                return lhs->match(media) and rhs->match(media);
            case Type::OR:
                return lhs->match(media) or rhs->match(media);
            default:
                return false;
            }
        }
    };

    struct _Prefix {
        enum struct Type {
            NOT,
            ONLY,
        };

        Type type;
        Box<MediaQuery> query;

        bool match(Media const &media) const {
            switch (type) {
            case Type::NOT:
                return not query->match(media);
            case Type::ONLY:
                return query->match(media);
            default:
                return false;
            }
        }
    };

    using _Store = Union<
        _Infix,
        _Prefix,
        Feature>;

    using enum _Infix::Type;
    using enum _Prefix::Type;

    _Store _store;

    MediaQuery(Feature feature) : _store(feature) {}

    MediaQuery(Meta::Convertible<Feature> auto &&feature)
        : _store(Feature{
              std::forward<decltype(feature)>(feature),
          }) {}

    MediaQuery(_Prefix::Type type, MediaQuery query)
        : _store(_Prefix{
              type,
              makeBox<MediaQuery>(query),
          }) {}

    MediaQuery(_Infix::Type type, MediaQuery lhs, MediaQuery rhs)
        : _store(_Infix{
              type,
              makeBox<MediaQuery>(lhs),
              makeBox<MediaQuery>(rhs),
          }) {}

    static MediaQuery negate(MediaQuery query) {
        return MediaQuery{NOT, query};
    }

    static MediaQuery only(MediaQuery query) {
        return MediaQuery{ONLY, query};
    }

    static MediaQuery combineAnd(MediaQuery lhs, MediaQuery rhs) {
        return MediaQuery{AND, lhs, rhs};
    }

    static MediaQuery combineOr(MediaQuery lhs, MediaQuery rhs) {
        return MediaQuery{OR, lhs, rhs};
    }

    bool match(Media const &media) const {
        return _store.visit([&](auto const &value) {
            return value.match(media);
        });
    }
};

} // namespace Web::Style

template <typename T>
struct Karm::Io::Formatter<Web::Style::RangeBound<T>> {
    Res<usize> format(Io::TextWriter &writer, Web::Style::RangeBound<T> const &v) {
        return Io::format(writer, "{} {}", v.value, v.type == Web::Style::RangeBound<T>::INCLUSIVE ? "inclusive"s : "exclusive"s);
    }
};

template <typename T, auto Web::Style::Media::*F>
struct Karm::Io::Formatter<Web::Style::RangeFeature<T, F>> {
    Res<usize> format(Io::TextWriter &writer, Web::Style::RangeFeature<T, F> const &v) {
        return Io::format(writer, "{} - {}", v.lower, v.upper);
    }
};

template <typename T, auto Web::Style::Media::*F>
struct Karm::Io::Formatter<Web::Style::DiscreteFeature<T, F>> {
    Res<usize> format(Io::TextWriter &writer, Web::Style::DiscreteFeature<T, F> const &v) {
        return Io::format(writer, "{}", v.value);
    }
};

template <>
struct Karm::Io::Formatter<Web::Style::Feature> {
    Res<usize> format(Io::TextWriter &writer, Web::Style::Feature const &val) {
        return val.visit([&](auto const &v) -> Res<usize> {
            return Io::format(writer, "{}", v);
        });
    }
};

Reflectable$(Web::Style::MediaQuery::_Infix, type, lhs, rhs);
Reflectable$(Web::Style::MediaQuery::_Prefix, type, query);
Reflectable$(Web::Style::MediaQuery, _store);
