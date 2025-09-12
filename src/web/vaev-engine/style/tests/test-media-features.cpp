#include <karm-math/au.h>
#include <karm-test/macros.h>

import Karm.Print;
import Vaev.Engine;

using namespace Karm;

namespace Vaev::Style::Tests {

static Media const TEST_MEDIA = {
    .type = MediaType::SCREEN,
    .width = 1920_au,
    .height = 1080_au,
    .aspectRatio = 16.0 / 9.0,
    .orientation = Print::Orientation::LANDSCAPE,

    .resolution = Resolution::fromDpi(96),
    .scan = Scan::PROGRESSIVE,
    .grid = false,
    .update = Update::NONE,
    .overflowBlock = OverflowBlock::NONE,
    .overflowInline = OverflowInline::NONE,

    .color = 8,
    .colorIndex = 256,
    .monochrome = 0,
    .colorGamut = ColorGamut::SRGB,
    .pointer = Pointer::NONE,
    .hover = Hover::NONE,
    .anyPointer = Pointer::FINE,
    .anyHover = Hover::HOVER,

    .prefersReducedMotion = ReducedMotion::REDUCE,
    .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
    .prefersContrast = Contrast::LESS,
    .forcedColors = Colors::NONE,
    .prefersColorScheme = ColorScheme::LIGHT,
    .prefersReducedData = ReducedData::REDUCE,

    .deviceWidth = 1920_au,
    .deviceHeight = 1080_au,
    .deviceAspectRatio = 16.0 / 9.0,
};

test$("feature-type") {
    expect$(TypeFeature{MediaType::SCREEN}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-width") {
    expect$(WidthFeature::min(1000_au).match(TEST_MEDIA));

    expect$(WidthFeature::max(2000_au).match(TEST_MEDIA));

    expect$(WidthFeature::exact(1920_au).match(TEST_MEDIA));

    return Ok();
}

test$("feature-height") {
    expect$(HeightFeature::min(1000_au).match(TEST_MEDIA));

    expect$(HeightFeature::max(2000_au).match(TEST_MEDIA));

    expect$(HeightFeature::exact(1080_au).match(TEST_MEDIA));

    return Ok();
}

test$("feature-aspect-ratio") {
    expect$(AspectRatioFeature::min(16.0 / 9.0).match(TEST_MEDIA));

    expect$(AspectRatioFeature::max(16.0 / 9.0).match(TEST_MEDIA));

    expect$(AspectRatioFeature::exact(16.0 / 9.0).match(TEST_MEDIA));

    return Ok();
}

test$("feature-orientation") {
    expect$(OrientationFeature{Print::Orientation::LANDSCAPE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-resolution") {
    expect$(ResolutionFeature::min(Resolution::fromDpi(96)).match(TEST_MEDIA));

    expect$(ResolutionFeature::max(Resolution::fromDpi(96)).match(TEST_MEDIA));

    expect$(ResolutionFeature::exact(Resolution::fromDpi(96)).match(TEST_MEDIA));

    return Ok();
}

test$("feature-scan") {
    expect$(ScanFeature{Scan::PROGRESSIVE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-grid") {
    expect$(GridFeature{false}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-update") {
    expect$(UpdateFeature{Update::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-block") {
    expect$(OverflowBlockFeature{OverflowBlock::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-inline") {
    expect$(OverflowInlineFeature{OverflowInline::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-color") {
    expect$(ColorFeature::min(8).match(TEST_MEDIA));

    expect$(ColorFeature::max(8).match(TEST_MEDIA));

    expect$(ColorFeature::exact(8).match(TEST_MEDIA));

    return Ok();
}

test$("feature-color-index") {
    expect$(ColorIndexFeature::min(256).match(TEST_MEDIA));

    expect$(ColorIndexFeature::max(256).match(TEST_MEDIA));

    expect$(ColorIndexFeature::exact(256).match(TEST_MEDIA));

    return Ok();
}

test$("feature-monochrome") {
    expect$(MonochromeFeature::min(0).match(TEST_MEDIA));

    expect$(MonochromeFeature::max(0).match(TEST_MEDIA));

    expect$(MonochromeFeature::exact(0).match(TEST_MEDIA));

    return Ok();
}

test$("feature-color-gamut") {
    expect$(ColorGamutFeature{ColorGamut::SRGB}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-pointer") {
    expect$(PointerFeature{Pointer::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-hover") {
    expect$(HoverFeature{Hover::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-pointer") {
    expect$(AnyPointerFeature{Pointer::FINE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-hover") {
    expect$(AnyHoverFeature{Hover::HOVER}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-motion") {
    expect$(PrefersReducedMotionFeature{ReducedMotion::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-transparency") {
    expect$(PrefersReducedTransparencyFeature{ReducedTransparency::NO_PREFERENCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-contrast") {
    expect$(PrefersContrastFeature{Contrast::LESS}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-forced-colors") {
    expect$(ForcedColorsFeature{Colors::NONE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-color-scheme") {
    expect$(PrefersColorSchemeFeature{ColorScheme::LIGHT}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-prefers-reduced-data") {
    expect$(PrefersReducedDataFeature{ReducedData::REDUCE}.match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-width") {
    expect$(DeviceWidthFeature::min(1920_au).match(TEST_MEDIA));

    expect$(DeviceWidthFeature::max(1920_au).match(TEST_MEDIA));

    expect$(DeviceWidthFeature::exact(1920_au).match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-height") {
    expect$(DeviceHeightFeature::min(1080_au).match(TEST_MEDIA));

    expect$(DeviceHeightFeature::max(1080_au).match(TEST_MEDIA));

    expect$(DeviceHeightFeature::exact(1080_au).match(TEST_MEDIA));

    return Ok();
}

test$("feature-device-aspect-ratio") {
    expect$(DeviceAspectRatioFeature::min(16.0 / 9.0).match(TEST_MEDIA));

    expect$(DeviceAspectRatioFeature::max(16.0 / 9.0).match(TEST_MEDIA));

    expect$(DeviceAspectRatioFeature::exact(16.0 / 9.0).match(TEST_MEDIA));

    return Ok();
}

} // namespace Vaev::Style::Tests
