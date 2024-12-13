#pragma once

namespace Vaev {

// MARK: Media Types -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#media-types

enum struct MediaType {
    ALL,
    PRINT,
    SCREEN,
    OTHER, // tty, tv, projection, handheld, braille, aural, embossed

    _LEN,
};

enum struct Scan {
    INTERLACE,
    PROGRESSIVE,

    _LEN,
};

enum struct Update {
    NONE,
    SLOW,
    FAST,

    _LEN,
};

enum struct OverflowBlock {
    NONE,
    SCROLL,
    PAGED,

    _LEN,
};

enum struct OverflowInline {
    NONE,
    SCROLL,

    _LEN,
};

enum struct ColorGamut {
    SRGB,
    DISPLAY_P3,
    REC2020,

    _LEN,
};

enum struct Pointer {
    NONE,
    COARSE,
    FINE,

    _LEN,
};

enum struct Hover {
    NONE,
    HOVER,

    _LEN,
};

enum struct ReducedMotion {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

enum struct ReducedTransparency {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

enum struct Contrast {
    NO_PREFERENCE,
    MORE,
    LESS,

    _LEN,
};

enum struct Colors {
    NONE,
    ACTIVE,

    _LEN,
};

enum struct ColorScheme {
    LIGHT,
    DARK,

    _LEN,
};

enum struct ReducedData {
    NO_PREFERENCE,
    REDUCE,

    _LEN,
};

} // namespace Vaev
