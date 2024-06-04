#include <karm-test/macros.h>
#include <web-media/features.h>

namespace Web::Media::Tests {

static Media const TEST_MEDIA = {
    .type = Type::SCREEN,
    .width = Types::Px{1920},
    .height = Types::Px{1080},
    .aspectRatio = 16.0 / 9.0,
    .orientation = Orientation::LANDSCAPE,

    .resolution = Types::Resolution::fromDpi(96),
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
};

test$("feature-type") {
    expect$(TypeFeature{Type::SCREEN}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-width") {
    expect$(WidthFeature::min(Types::Px{1000})
                .match(TEST_MEDIA));

    expect$(WidthFeature::max(Types::Px{2000})
                .match(TEST_MEDIA));

    expect$(WidthFeature::exact(Types::Px{1920})
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-height") {
    expect$(HeightFeature::min(Types::Px{1000})
                .match(TEST_MEDIA));

    expect$(HeightFeature::max(Types::Px{2000})
                .match(TEST_MEDIA));

    expect$(HeightFeature::exact(Types::Px{1080})
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-aspect-ratio") {
    expect$(AspectRatioFeature::min(16.0 / 9.0)
                .match(TEST_MEDIA));

    expect$(AspectRatioFeature::max(16.0 / 9.0)
                .match(TEST_MEDIA));

    expect$(AspectRatioFeature::exact(16.0 / 9.0)
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-orientation") {
    expect$(OrientationFeature{Orientation::LANDSCAPE}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-resolution") {
    expect$(ResolutionFeature::min(Types::Resolution::fromDpi(96))
                .match(TEST_MEDIA));

    expect$(ResolutionFeature::max(Types::Resolution::fromDpi(96))
                .match(TEST_MEDIA));

    expect$(ResolutionFeature::exact(Types::Resolution::fromDpi(96))
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-scan") {
    expect$(ScanFeature{Scan::PROGRESSIVE}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-grid") {
    expect$(GridFeature{false}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-update") {
    expect$(UpdateFeature{Update::NONE}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-block") {
    expect$(OverflowBlockFeature{OverflowBlock::NONE}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-overflow-inline") {
    expect$(OverflowInlineFeature{OverflowInline::NONE}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-color") {
    expect$(ColorFeature::min(8)
                .match(TEST_MEDIA));

    expect$(ColorFeature::max(8)
                .match(TEST_MEDIA));

    expect$(ColorFeature::exact(8)
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-color-index") {
    expect$(ColorIndexFeature::min(256)
                .match(TEST_MEDIA));

    expect$(ColorIndexFeature::max(256)
                .match(TEST_MEDIA));

    expect$(ColorIndexFeature::exact(256)
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-monochrome") {
    expect$(MonochromeFeature::min(0)
                .match(TEST_MEDIA));

    expect$(MonochromeFeature::max(0)
                .match(TEST_MEDIA));

    expect$(MonochromeFeature::exact(0)
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-color-gamut") {
    expect$(ColorGamutFeature{ColorGamut::SRGB}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-pointer") {
    expect$(PointerFeature{Pointer::NONE}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-hover") {
    expect$(HoverFeature{Hover::NONE}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-pointer") {
    expect$(AnyPointerFeature{Pointer::FINE}
                .match(TEST_MEDIA));

    return Ok();
}

test$("feature-any-hover") {
    expect$(AnyHoverFeature{Hover::HOVER}
                .match(TEST_MEDIA));

    return Ok();
}

} // namespace Web::Media::Tests
