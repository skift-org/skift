#pragma once

#include <karm-base/box.h>
#include <karm-print/paper.h>
#include <vaev-base/length.h>
#include <vaev-base/media.h>
#include <vaev-base/numbers.h>
#include <vaev-base/resolution.h>

#include "css/parser.h"

namespace Vaev::Style {

// MARK: Media -----------------------------------------------------------------

struct Media {
    /// 2.3. Media Types
    /// https://drafts.csswg.org/mediaqueries/#media-types
    MediaType type;

    // 4. MARK: Viewport/Page Dimensions Media Features

    /// 4.1. Width: the width feature
    /// https://drafts.csswg.org/mediaqueries/#width
    Au width;

    /// 4.2. Height: the height feature
    /// https://drafts.csswg.org/mediaqueries/#height
    Au height;

    /// 4.3. Device Width: the device-width feature
    /// https://drafts.csswg.org/mediaqueries/#aspect-ratio
    Number aspectRatio;

    /// 4.4. Orientation: the orientation feature
    /// https://drafts.csswg.org/mediaqueries/#orientation
    Print::Orientation orientation;

    // 5. MARK: Display Quality Media Features

    /// 5.1. Resolution: the resolution feature
    /// https://drafts.csswg.org/mediaqueries/#resolution
    Resolution resolution;

    /// 5.2. Scan: the scan feature
    /// https://drafts.csswg.org/mediaqueries/#scan
    Scan scan = Scan::PROGRESSIVE;

    /// 5.3. Grid: the grid feature
    /// https://drafts.csswg.org/mediaqueries/#grid
    bool grid = false;

    /// 5.4. Update: the update feature
    /// https://drafts.csswg.org/mediaqueries/#update
    Update update;

    /// 5.5. Overflow Block: the overflow-block feature
    /// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
    OverflowBlock overflowBlock;

    /// 5.6. Overflow Inline: the overflow-inline feature
    /// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
    OverflowInline overflowInline;

    //  6. MARK: Color Media Features

    // 6.1. Color: the color feature
    /// https://drafts.csswg.org/mediaqueries/#color
    Integer color;

    /// 6.2. Color Index: the color-index feature
    /// https://drafts.csswg.org/mediaqueries/#color-index
    Integer colorIndex;

    /// 6.3. Monochrome: the monochrome feature
    /// https://drafts.csswg.org/mediaqueries/#monochrome
    Integer monochrome;

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
    /// https://drafts.csswg.org/mediaqueries/#any-input
    Pointer anyPointer;

    /// 7.4. Any Hover: the any-hover feature
    /// https://drafts.csswg.org/mediaqueries/#any-input
    Hover anyHover;

    // 11. MARK: User Preference Media Features

    // 11.1. Detecting the desire for less motion on the page:
    //       the prefers-reduced-motion feature
    //
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-motion
    ReducedMotion prefersReducedMotion;

    // 11.2. Detecting the desire for reduced transparency on the page:
    //       the prefers-reduced-transparency feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-transparency
    ReducedTransparency prefersReducedTransparency;

    // 11.3. Detecting the desire for increased or decreased color contrast
    //       from elements on the page: the prefers-contrast feature
    //
    // https://drafts.csswg.org/mediaqueries-5/#prefers-contrast
    Contrast prefersContrast;

    // 11.4. Detecting Forced Colors Mode: the forced-colors feature
    // https://drafts.csswg.org/mediaqueries-5/#forced-colors
    Colors forcedColors;

    // 11.5. Detecting the desire for light or dark color schemes:
    //       the prefers-color-scheme feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-color-scheme
    ColorScheme prefersColorScheme;

    // 11.6. Detecting the desire for reduced data usage when loading a page:
    //       the prefers-reduced-data feature
    // https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-data
    ReducedData prefersReducedData;

    // Appendix A: Deprecated Media Features
    Au deviceWidth;
    Au deviceHeight;
    Number deviceAspectRatio;
};

// MARK: Media Features --------------------------------------------------------

enum struct RangePrefix {
    MIN,   // min-<feature> : value
    MAX,   // max-<feature> : value
    EXACT, // <feature> : value
    BOOL,  // <feature>

    _LEN,
};

template <typename T>
struct RangeBound {
    enum Type : u8 {
        NONE,
        INCLUSIVE,
        EXCLUSIVE,

        _LEN,
    };

    T value = {};
    Type type = NONE;

    void repr(Io::Emit& e) const {
        e(
            "{}{}",
            value,
            type == INCLUSIVE   ? "i"
            : type == EXCLUSIVE ? "e"
                                : ""
        );
    }
};

template <StrLit NAME, typename T, auto Media::* F>
struct RangeFeature {
    using Bound = RangeBound<T>;
    using Inner = T;

    Bound lower{};
    Bound upper{};

    static constexpr Str name() {
        return NAME;
    }

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

        // both types are NONE, evaluate in boolean context
        if (not lower.type and not upper.type) {
            result = actual != T{};
        }

        return result;
    }

    bool match(Media const& media) const {
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

    static RangeFeature boolean() {
        return {};
    }

    static RangeFeature make(RangePrefix prefix, T value = {}) {
        switch (prefix) {
        case RangePrefix::MIN:
            return min(value);
        case RangePrefix::MAX:
            return max(value);
        case RangePrefix::EXACT:
            return exact(value);
        case RangePrefix::BOOL:
            return boolean();

        default:
            unreachable();
        }
    }

    void repr(Io::Emit& e) const {
        e("{}: {} - {}", NAME, lower, upper);
    }
};

template <StrLit NAME, typename T, auto Media::* F>
struct DiscreteFeature {
    using Inner = T;

    enum Type : u8 {
        NONE,
        EQUAL,

        _LEN,
    };

    T value{};
    Type type = EQUAL;

    static constexpr Str name() {
        return NAME;
    }

    static DiscreteFeature make(RangePrefix prefix, T value = {}) {
        if (prefix == RangePrefix::BOOL)
            return {.type = NONE};
        return {value};
    }

    bool match(T actual) const {
        if (type == Type::NONE)
            return actual != T{};
        return actual == value;
    }

    bool match(Media const& media) const {
        return match(media.*F);
    }

    void repr(Io::Emit& e) const {
        e("({}: {} {})", NAME, type, value);
    }
};

/// 2.3. Media Types
/// https://drafts.csswg.org/mediaqueries/#media-types
using TypeFeature = DiscreteFeature<"type", MediaType, &Media::type>;

// 4. MARK: Viewport/Page Dimensions Media Features ----------------------------

/// 4.1. Width: the width feature
/// https://drafts.csswg.org/mediaqueries/#width
using WidthFeature = RangeFeature<"width", Length, &Media::width>;

/// 4.2. Height: the height feature
/// https://drafts.csswg.org/mediaqueries/#height
using HeightFeature = RangeFeature<"height", Length, &Media::height>;

/// 4.3. Aspect-Ratio: the aspect-ratio feature
/// https://drafts.csswg.org/mediaqueries/#aspect-ratio
using AspectRatioFeature = RangeFeature<"aspect-ratio", Number, &Media::aspectRatio>;

/// 4.4. Orientation: the orientation feature
/// https://drafts.csswg.org/mediaqueries/#orientation
using OrientationFeature = DiscreteFeature<"orientation", Print::Orientation, &Media::orientation>;

// 5. MARK: Display Quality Media Features

/// 5.1. Resolution: the resolution feature
/// https://drafts.csswg.org/mediaqueries/#resolution
using ResolutionFeature = RangeFeature<"resolution", Resolution, &Media::resolution>;

/// 5.2. Scan: the scan feature
/// https://drafts.csswg.org/mediaqueries/#scan
using ScanFeature = DiscreteFeature<"scan", Scan, &Media::scan>;

/// 5.3. Grid: the grid feature
/// https://drafts.csswg.org/mediaqueries/#grid
using GridFeature = DiscreteFeature<"grid", bool, &Media::grid>;

/// 5.4. Update: the update feature
/// https://drafts.csswg.org/mediaqueries/#update
using UpdateFeature = DiscreteFeature<"update", Update, &Media::update>;

/// 5.5. Overflow Block: the overflow-block feature
/// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
using OverflowBlockFeature = DiscreteFeature<"overflow-block", OverflowBlock, &Media::overflowBlock>;

/// 5.6. Overflow Inline: the overflow-inline feature
/// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
using OverflowInlineFeature = DiscreteFeature<"overflow-inline", OverflowInline, &Media::overflowInline>;

//  6. MARK: Color Media Features ----------------------------------------------

// 6.1. Color: the color feature
/// https://drafts.csswg.org/mediaqueries/#color
using ColorFeature = RangeFeature<"color", Integer, &Media::color>;

/// 6.2. Color Index: the color-index feature
/// https://drafts.csswg.org/mediaqueries/#color-index
using ColorIndexFeature = RangeFeature<"color-index", Integer, &Media::colorIndex>;

/// 6.3. Monochrome: the monochrome feature
/// https://drafts.csswg.org/mediaqueries/#monochrome
using MonochromeFeature = RangeFeature<"monochrome", Integer, &Media::monochrome>;

/// 6.4. Color Gamut: the color-gamut feature
/// https://drafts.csswg.org/mediaqueries/#color-gamut
using ColorGamutFeature = DiscreteFeature<"color-gamut", ColorGamut, &Media::colorGamut>;

// 7. MARK: Interaction Media Features

/// 7.1. Pointer: the pointer feature
/// https://drafts.csswg.org/mediaqueries/#pointer
using PointerFeature = DiscreteFeature<"pointer", Pointer, &Media::pointer>;

/// 7.2. Hover: the hover feature
/// https://drafts.csswg.org/mediaqueries/#hover
using HoverFeature = DiscreteFeature<"hover", Hover, &Media::hover>;

/// 7.3. Any Pointer: the any-pointer feature
/// https://drafts.csswg.org/mediaqueries/#any-pointer
using AnyPointerFeature = DiscreteFeature<"pointer", Pointer, &Media::anyPointer>;

/// 7.4. Any Hover: the any-hover feature
/// https://drafts.csswg.org/mediaqueries/#any-hover
using AnyHoverFeature = DiscreteFeature<"hover", Hover, &Media::anyHover>;

// 11. MARK: User Preference Media Features ------------------------------------

// 11.1. Detecting the desire for less motion on the page:
//       the prefers-reduced-motion feature
//
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-motion
using PrefersReducedMotionFeature = DiscreteFeature<"prefers-reduced-motion", ReducedMotion, &Media::prefersReducedMotion>;

// 11.2. Detecting the desire for reduced transparency on the page:
//       the prefers-reduced-transparency feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-transparency
using PrefersReducedTransparencyFeature = DiscreteFeature<"prefers-reduced-transparency", ReducedTransparency, &Media::prefersReducedTransparency>;

// 11.3. Detecting the desire for increased or decreased color contrast
//       from elements on the page: the prefers-contrast feature
//
// https://drafts.csswg.org/mediaqueries-5/#prefers-contrast
using PrefersContrastFeature = DiscreteFeature<"prefers-contrast", Contrast, &Media::prefersContrast>;

// 11.4. Detecting Forced Colors Mode: the forced-colors feature
// https://drafts.csswg.org/mediaqueries-5/#forced-colors
using ForcedColorsFeature = DiscreteFeature<"forced-colors", Colors, &Media::forcedColors>;

// 11.5. Detecting the desire for light or dark color schemes:
//       the prefers-color-scheme feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-color-scheme
using PrefersColorSchemeFeature = DiscreteFeature<"prefers-color-scheme", ColorScheme, &Media::prefersColorScheme>;

// 11.6. Detecting the desire for reduced data usage when loading a page:
//       the prefers-reduced-data feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-data
using PrefersReducedDataFeature = DiscreteFeature<"prefers-reduced-data", ReducedData, &Media::prefersReducedData>;

// Appendix A: Deprecated Media Features
using DeviceWidthFeature = RangeFeature<"device-width", Length, &Media::deviceWidth>;
using DeviceHeightFeature = RangeFeature<"device-height", Length, &Media::deviceHeight>;
using DeviceAspectRatioFeature = RangeFeature<"device-aspect-ratio", Number, &Media::deviceAspectRatio>;

// MARK: Media Feature ---------------------------------------------------------

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
    AnyHoverFeature,
    PrefersReducedMotionFeature,
    PrefersReducedTransparencyFeature,
    PrefersContrastFeature,
    ForcedColorsFeature,
    PrefersColorSchemeFeature,
    PrefersReducedDataFeature,
    DeviceWidthFeature,
    DeviceHeightFeature,
    DeviceAspectRatioFeature>;

struct Feature : public _Feature {
    using _Feature::_Feature;

    void repr(Io::Emit& e) const {
        visit([&](auto const& feature) {
            e("{}", feature);
        });
    }

    bool match(Media const& media) const {
        return visit([&](auto const& feature) {
            return feature.match(media);
        });
    }

    static Feature type(MediaType value) {
        return TypeFeature{value};
    }

    static Feature width(WidthFeature value) {
        return value;
    }
};

// MARK: Media Queries ---------------------------------------------------------

struct MediaQuery {
    struct _Infix {
        enum struct Type {
            AND,
            OR,

            _LEN0,
        };

        Type type;
        Box<MediaQuery> lhs;
        Box<MediaQuery> rhs;

        bool match(Media const& media) const {
            switch (type) {
            case Type::AND:
                return lhs->match(media) and rhs->match(media);
            case Type::OR:
                return lhs->match(media) or rhs->match(media);
            default:
                return false;
            }
        }

        void repr(Io::Emit& e) const {
            e("({} {} {})", *lhs, type, *rhs);
        }
    };

    struct _Prefix {
        enum struct Type {
            NOT,
            ONLY,

            _LEN1,
        };

        Type type;
        Box<MediaQuery> query;

        bool match(Media const& media) const {
            switch (type) {
            case Type::NOT:
                return not query->match(media);
            case Type::ONLY:
                return query->match(media);
            default:
                return false;
            }
        }

        void repr(Io::Emit& e) const {
            e("({} {})", type, *query);
        }
    };

    using _Store = Union<
        None,
        _Infix,
        _Prefix,
        Feature>;

    using enum _Infix::Type;
    using enum _Prefix::Type;

    _Store _store = NONE;

    MediaQuery() = default;

    MediaQuery(None) : _store(None{}) {}

    MediaQuery(Feature feature) : _store(feature) {}

    MediaQuery(Meta::Convertible<Feature> auto&& feature)
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

    bool match(Media const& media) const {
        return _store.visit(Visitor{[&](auto const& value) {
                                        return value.match(media);
                                    },
                                    [](None) {
                                        return true;
                                    }});
    }

    void repr(Io::Emit& e) const {
        _store.visit(Visitor{[&](auto const& value) {
                                 e("{}", value);
                             },
                             [&](None) {
                                 e("all");
                             }});
    }
};

// MARK: Parser ----------------------------------------------------------------

MediaQuery parseMediaQuery(Cursor<Css::Sst>& c);

} // namespace Vaev::Style
