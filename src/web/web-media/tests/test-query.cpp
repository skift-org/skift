#include <karm-test/macros.h>
#include <web-media/query.h>

namespace Web::Media::Tests {

static Media const TEST_MEDIA = {
    .type = Type::SCREEN,
    .width = Unit::Px(1920),
    .height = Unit::Px(1080),
    .aspectRatio = 16.0 / 9.0,
    .orientation = Orientation::LANDSCAPE,

    .resolution = Unit::Resolution::fromDpi(96),
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

test$("logical-and") {
    auto query = Query::combineAnd(
        TypeFeature{Type::SCREEN},
        WidthFeature::min(Unit::Px{1920})
    );

    expect$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-or") {
    auto query = Query::combineOr(
        TypeFeature{Type::SCREEN},
        WidthFeature::min(Unit::Px{1920})
    );

    expect$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-not") {
    auto query = Query::negate(TypeFeature{Type::PRINT});

    expect$(query.match(TEST_MEDIA));

    return Ok();
}

test$("logical-only") {
    auto query = Query::only(TypeFeature{Type::SCREEN});

    expect$(query.match(TEST_MEDIA));

    return Ok();
}

} // namespace Web::Media::Tests
