#pragma once

#include "types.h"

namespace Web::Media {

struct Media {
    /// 2.3. Media Types
    /// https://drafts.csswg.org/mediaqueries/#media-types
    Type type;

    // 4. MARK: Viewport/Page Dimensions Media Features

    /// 4.1. Width: the width feature
    /// https://drafts.csswg.org/mediaqueries/#width
    Css::Length width;

    /// 4.2. Height: the height feature
    /// https://drafts.csswg.org/mediaqueries/#height
    Css::Length height;

    /// 4.3. Device Width: the device-width feature
    /// https://drafts.csswg.org/mediaqueries/#aspect-ratio
    double aspectRatio;

    /// 4.4. Device Height: the device-height feature
    /// https://drafts.csswg.org/mediaqueries/#orientation
    Orientation orientation;

    // 5. MARK: Display Quality Media Features

    /// 5.1. Resolution: the resolution feature
    /// https://drafts.csswg.org/mediaqueries/#resolution
    Css::Resolution resolution;

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

} // namespace Web::Media
