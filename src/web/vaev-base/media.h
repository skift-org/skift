#pragma once

namespace Vaev {

// MARK: Media Types -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#media-types

enum struct MediaType {
    ALL,
    PRINT,
    SCREEN,

    OTHER, // tty, tv, projection, handheld, braille, aural, embossed
};

enum struct Orientation {
    PORTRAIT,
    LANDSCAPE,
};

enum struct Scan {
    INTERLACE,
    PROGRESSIVE,
};

enum struct Update {
    NONE,
    SLOW,
    FAST,
};

enum struct OverflowBlock {
    NONE,
    SCROLL,
    PAGED,
};

enum struct OverflowInline {
    NONE,
    SCROLL,
};

enum struct ColorGamut {
    SRGB,
    DISPLAY_P3,
    REC2020,
};

enum struct Pointer {
    NONE,
    COARSE,
    FINE,
};

enum struct Hover {
    NONE,
    HOVER,
};

} // namespace Vaev
