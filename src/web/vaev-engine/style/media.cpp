module;

#include <karm-logger/logger.h>
#include <karm-math/au.h>

export module Vaev.Engine:style.media;

import Karm.Core;
import Karm.Print;
import :values;
import :css;

using namespace Karm;

namespace Vaev::Style {

// MARK: Media -----------------------------------------------------------------

export struct Media {
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

    static Media forView(Math::Vec2i viewport, ColorScheme colorScheme) {
        return {
            .type = MediaType::SCREEN,
            .width = Au{viewport.width},
            .height = Au{viewport.height},
            .aspectRatio = viewport.width / (f64)viewport.height,
            .orientation = Print::Orientation::LANDSCAPE,

            .resolution = Resolution::fromDpi(96),
            .scan = Scan::PROGRESSIVE,
            .grid = false,
            .update = Update::FAST,

            .overflowBlock = OverflowBlock::SCROLL,
            .overflowInline = OverflowInline::SCROLL,

            .color = 8,
            .colorIndex = 0,
            .monochrome = 0,
            .colorGamut = ColorGamut::SRGB,
            .pointer = Pointer::FINE,
            .hover = Hover::HOVER,
            .anyPointer = Pointer::FINE,
            .anyHover = Hover::HOVER,

            .prefersReducedMotion = ReducedMotion::NO_PREFERENCE,
            .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
            .prefersContrast = Contrast::NO_PREFERENCE,
            .forcedColors = Colors::NONE,
            .prefersColorScheme = colorScheme,
            .prefersReducedData = ReducedData::NO_PREFERENCE,

            // NOTE: Deprecated Media Features
            .deviceWidth = Au{viewport.width},
            .deviceHeight = Au{viewport.height},
            .deviceAspectRatio = viewport.width / (f64)viewport.height,
        };
    }

    static Media forRender(Vec2Au viewport, Resolution scale) {
        return {
            .type = MediaType::SCREEN,
            .width = viewport.width,
            .height = viewport.height,
            .aspectRatio = Number{viewport.width} / Number{viewport.height},
            .orientation = Print::Orientation::PORTRAIT,

            .resolution = scale,
            .scan = Scan::PROGRESSIVE,
            .grid = false,
            .update = Update::NONE,

            .overflowBlock = OverflowBlock::NONE,
            .overflowInline = OverflowInline::NONE,

            .color = 8,
            .colorIndex = 0,
            .monochrome = 0,
            .colorGamut = ColorGamut::SRGB,
            .pointer = Pointer::NONE,
            .hover = Hover::NONE,
            .anyPointer = Pointer::NONE,
            .anyHover = Hover::NONE,

            .prefersReducedMotion = ReducedMotion::REDUCE,
            .prefersReducedTransparency = ReducedTransparency::REDUCE,
            .prefersContrast = Contrast::MORE,
            .forcedColors = Colors::NONE,
            .prefersColorScheme = ColorScheme::LIGHT,
            .prefersReducedData = ReducedData::NO_PREFERENCE,

            // NOTE: Deprecated Media Features
            .deviceWidth = viewport.width,
            .deviceHeight = viewport.height,
            .deviceAspectRatio = Number{viewport.width} / Number{viewport.height},
        };
    }

    static Media forPrint(Print::Settings const& settings) {
        return {
            .type = MediaType::PRINT,
            .width = Au{settings.paper.width},
            .height = Au{settings.paper.height},
            .aspectRatio = settings.paper.width / f64{settings.paper.height},
            .orientation = settings.orientation,

            .resolution = Resolution{settings.scale, Resolution::X},
            .scan = Scan::PROGRESSIVE,
            .grid = false,
            .update = Update::FAST,

            .overflowBlock = OverflowBlock::SCROLL,
            .overflowInline = OverflowInline::SCROLL,

            .color = 8,
            .colorIndex = 0,
            .monochrome = 0,
            .colorGamut = ColorGamut::SRGB,
            .pointer = Pointer::FINE,
            .hover = Hover::HOVER,
            .anyPointer = Pointer::FINE,
            .anyHover = Hover::HOVER,

            .prefersReducedMotion = ReducedMotion::NO_PREFERENCE,
            .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
            .prefersContrast = Contrast::NO_PREFERENCE,
            .forcedColors = Colors::NONE,
            .prefersColorScheme = ColorScheme::LIGHT,
            .prefersReducedData = ReducedData::NO_PREFERENCE,

            // NOTE: Deprecated Media Features
            .deviceWidth = Au{settings.paper.width},
            .deviceHeight = Au{settings.paper.height},
            .deviceAspectRatio = settings.paper.width / settings.paper.height,
        };
    }
};

// MARK: Media Features --------------------------------------------------------

export enum struct RangePrefix {
    MIN,   // min-<feature> : value
    MAX,   // max-<feature> : value
    EXACT, // <feature> : value
    BOOL,  // <feature>

    _LEN,
};

export template <typename T>
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

export template <StrLit NAME, typename T, auto Media::* F>
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

export template <StrLit NAME, typename T, auto Media::* F>
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
export using TypeFeature = DiscreteFeature<"type", MediaType, &Media::type>;

// 4. MARK: Viewport/Page Dimensions Media Features ----------------------------

/// 4.1. Width: the width feature
/// https://drafts.csswg.org/mediaqueries/#width
export using WidthFeature = RangeFeature<"width", Length, &Media::width>;

/// 4.2. Height: the height feature
/// https://drafts.csswg.org/mediaqueries/#height
export using HeightFeature = RangeFeature<"height", Length, &Media::height>;

/// 4.3. Aspect-Ratio: the aspect-ratio feature
/// https://drafts.csswg.org/mediaqueries/#aspect-ratio
export using AspectRatioFeature = RangeFeature<"aspect-ratio", Number, &Media::aspectRatio>;

/// 4.4. Orientation: the orientation feature
/// https://drafts.csswg.org/mediaqueries/#orientation
export using OrientationFeature = DiscreteFeature<"orientation", Print::Orientation, &Media::orientation>;

// 5. MARK: Display Quality Media Features

/// 5.1. Resolution: the resolution feature
/// https://drafts.csswg.org/mediaqueries/#resolution
export using ResolutionFeature = RangeFeature<"resolution", Resolution, &Media::resolution>;

/// 5.2. Scan: the scan feature
/// https://drafts.csswg.org/mediaqueries/#scan
export using ScanFeature = DiscreteFeature<"scan", Scan, &Media::scan>;

/// 5.3. Grid: the grid feature
/// https://drafts.csswg.org/mediaqueries/#grid
export using GridFeature = DiscreteFeature<"grid", bool, &Media::grid>;

/// 5.4. Update: the update feature
/// https://drafts.csswg.org/mediaqueries/#update
export using UpdateFeature = DiscreteFeature<"update", Update, &Media::update>;

/// 5.5. Overflow Block: the overflow-block feature
/// https://drafts.csswg.org/mediaqueries/#mf-overflow-block
export using OverflowBlockFeature = DiscreteFeature<"overflow-block", OverflowBlock, &Media::overflowBlock>;

/// 5.6. Overflow Inline: the overflow-inline feature
/// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline
export using OverflowInlineFeature = DiscreteFeature<"overflow-inline", OverflowInline, &Media::overflowInline>;

//  6. MARK: Color Media Features ----------------------------------------------

// 6.1. Color: the color feature
/// https://drafts.csswg.org/mediaqueries/#color
export using ColorFeature = RangeFeature<"color", Integer, &Media::color>;

/// 6.2. Color Index: the color-index feature
/// https://drafts.csswg.org/mediaqueries/#color-index
export using ColorIndexFeature = RangeFeature<"color-index", Integer, &Media::colorIndex>;

/// 6.3. Monochrome: the monochrome feature
/// https://drafts.csswg.org/mediaqueries/#monochrome
export using MonochromeFeature = RangeFeature<"monochrome", Integer, &Media::monochrome>;

/// 6.4. Color Gamut: the color-gamut feature
/// https://drafts.csswg.org/mediaqueries/#color-gamut
export using ColorGamutFeature = DiscreteFeature<"color-gamut", ColorGamut, &Media::colorGamut>;

// 7. MARK: Interaction Media Features

/// 7.1. Pointer: the pointer feature
/// https://drafts.csswg.org/mediaqueries/#pointer
export using PointerFeature = DiscreteFeature<"pointer", Pointer, &Media::pointer>;

/// 7.2. Hover: the hover feature
/// https://drafts.csswg.org/mediaqueries/#hover
export using HoverFeature = DiscreteFeature<"hover", Hover, &Media::hover>;

/// 7.3. Any Pointer: the any-pointer feature
/// https://drafts.csswg.org/mediaqueries/#any-pointer
export using AnyPointerFeature = DiscreteFeature<"pointer", Pointer, &Media::anyPointer>;

/// 7.4. Any Hover: the any-hover feature
/// https://drafts.csswg.org/mediaqueries/#any-hover
export using AnyHoverFeature = DiscreteFeature<"hover", Hover, &Media::anyHover>;

// 11. MARK: User Preference Media Features ------------------------------------

// 11.1. Detecting the desire for less motion on the page:
//       the prefers-reduced-motion feature
//
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-motion
export using PrefersReducedMotionFeature = DiscreteFeature<"prefers-reduced-motion", ReducedMotion, &Media::prefersReducedMotion>;

// 11.2. Detecting the desire for reduced transparency on the page:
//       the prefers-reduced-transparency feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-transparency
export using PrefersReducedTransparencyFeature = DiscreteFeature<"prefers-reduced-transparency", ReducedTransparency, &Media::prefersReducedTransparency>;

// 11.3. Detecting the desire for increased or decreased color contrast
//       from elements on the page: the prefers-contrast feature
//
// https://drafts.csswg.org/mediaqueries-5/#prefers-contrast
export using PrefersContrastFeature = DiscreteFeature<"prefers-contrast", Contrast, &Media::prefersContrast>;

// 11.4. Detecting Forced Colors Mode: the forced-colors feature
// https://drafts.csswg.org/mediaqueries-5/#forced-colors
export using ForcedColorsFeature = DiscreteFeature<"forced-colors", Colors, &Media::forcedColors>;

// 11.5. Detecting the desire for light or dark color schemes:
//       the prefers-color-scheme feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-color-scheme
export using PrefersColorSchemeFeature = DiscreteFeature<"prefers-color-scheme", ColorScheme, &Media::prefersColorScheme>;

// 11.6. Detecting the desire for reduced data usage when loading a page:
//       the prefers-reduced-data feature
// https://drafts.csswg.org/mediaqueries-5/#prefers-reduced-data
export using PrefersReducedDataFeature = DiscreteFeature<"prefers-reduced-data", ReducedData, &Media::prefersReducedData>;

// Appendix A: Deprecated Media Features
export using DeviceWidthFeature = RangeFeature<"device-width", Length, &Media::deviceWidth>;
export using DeviceHeightFeature = RangeFeature<"device-height", Length, &Media::deviceHeight>;
export using DeviceAspectRatioFeature = RangeFeature<"device-aspect-ratio", Number, &Media::deviceAspectRatio>;

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

export struct Feature : _Feature {
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

export struct MediaQuery {
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
            case AND:
                return lhs->match(media) and rhs->match(media);
            case OR:
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
            case NOT:
                return not query->match(media);
            case ONLY:
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

Pair<RangePrefix, Str> _explodeFeatureName(Io::SScan s) {
    if (s.skip("min-"))
        return {RangePrefix::MIN, s.remStr()};
    else if (s.skip("max-"))
        return {RangePrefix::MAX, s.remStr()};
    else
        return {RangePrefix::EXACT, s.remStr()};
}

Feature _parseMediaFeature(Cursor<Css::Sst>& c) {
    if (c.ended()) {
        logWarn("unexpected end of input");
        return TypeFeature{MediaType::OTHER};
    }

    if (*c != Css::Token::IDENT) {
        logWarn("expected ident");
        return TypeFeature{MediaType::OTHER};
    }

    auto unexplodedName = c.next().token.data;
    auto [prefix, name] = _explodeFeatureName(unexplodedName.str());

    Opt<Feature> prop;

    eatWhitespace(c);
    Feature::any([&]<typename F>() -> bool {
        if (name != F::name())
            return false;

        if (not c.skip(Css::Token::COLON)) {
            prop.emplace(F::make(RangePrefix::BOOL));
            return true;
        }

        eatWhitespace(c);

        auto maybeValue = parseValue<typename F::Inner>(c);
        if (not maybeValue) {
            logWarn("failed to parse value for feature {#}: {}", F::name(), maybeValue.none());
            return true;
        }

        prop.emplace(F::make(prefix, maybeValue.take()));
        return true;
    });

    if (not prop) {
        logWarn("cannot parse feature: {}", unexplodedName);
        return TypeFeature{MediaType::OTHER};
    }

    return prop.take();
}

MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst>& c);

MediaQuery _parseMediaQueryLeaf(Cursor<Css::Sst>& c) {
    if (c.skip(Css::Token::ident("not"))) {
        return MediaQuery::negate(_parseMediaQueryInfix(c));
    } else if (c.skip(Css::Token::ident("only"))) {
        return _parseMediaQueryInfix(c);
    } else if (c.peek() == Css::Sst::BLOCK) {
        Cursor<Css::Sst> content = c.next().content;
        return _parseMediaQueryInfix(content);
    } else if (auto type = parseValue<MediaType>(c)) {
        return TypeFeature{type.take()};
    } else
        return _parseMediaFeature(c);
}

MediaQuery _parseMediaQueryInfix(Cursor<Css::Sst>& c) {
    auto lhs = _parseMediaQueryLeaf(c);

    eatWhitespace(c);
    while (not c.ended()) {
        if (c.skip(Css::Token::ident("and"))) {
            lhs = MediaQuery::combineAnd(lhs, _parseMediaQueryLeaf(c));
        } else if (c.skip(Css::Token::ident("or"))) {
            lhs = MediaQuery::combineOr(lhs, _parseMediaQueryLeaf(c));
        } else {
            break;
        }
    }
    return lhs;
}

// https://drafts.csswg.org/mediaqueries/#mq-syntax
// https://drafts.csswg.org/mediaqueries/#typedef-media-query-list
export MediaQuery parseMediaQuery(Cursor<Css::Sst>& c) {
    eatWhitespace(c);

    // This definition of <media-query-list> parsing intentionally accepts an empty list.
    if (c.ended())
        return {};

    MediaQuery lhs = _parseMediaQueryInfix(c);
    eatWhitespace(c);
    while (not c.ended() and c.skip(Css::Token::COMMA)) {
        eatWhitespace(c);
        auto rhs = _parseMediaQueryInfix(c);
        lhs = MediaQuery::combineOr(lhs, rhs);
        eatWhitespace(c);
    }

    return lhs;
}

} // namespace Vaev::Style
