#include <karm-sys/entry.h>
#include <karm-sys/time.h>
#include <vaev-driver/fetcher.h>
#include <vaev-driver/render.h>

namespace Vaev {

Style::Media constructMedia(Vec2Px pageSize) {
    return {
        .type = MediaType::SCREEN,
        .width = Px{pageSize.width},
        .height = Px{pageSize.height},
        .aspectRatio = pageSize.width.toFloat<Number>() / pageSize.height.toFloat<Number>(),
        .orientation = Orientation::LANDSCAPE,

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

        .prefersReducedMotion = ReducedMotion::REDUCE,
        .prefersReducedTransparency = ReducedTransparency::NO_PREFERENCE,
        .prefersContrast = Contrast::NO_PREFERENCE,
        .forcedColors = Colors::NONE,
        .prefersColorScheme = ColorScheme::LIGHT,
        .prefersReducedData = ReducedData::NO_PREFERENCE,
    };
}

} // namespace Vaev

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto args = Sys::useArgs(ctx);
    if (args.len() != 2) {
        Sys::errln("usage: html2pdf <input.html> <output.pdf>\n");
        co_return Error::invalidInput();
    }

    auto input = co_try$(Mime::parseUrlOrPath(args[0]));
    auto dom = co_try$(Vaev::Driver::fetchDocument(input));

    Vaev::Vec2Px viewport{Vaev::Px{800}, Vaev::Px{600}};
    auto media = Vaev::constructMedia(viewport);

    auto start = Sys::now();
    auto result = Vaev::Driver::render(*dom, media, viewport);
    auto elapsed = Sys::now() - start;
    logDebug("render time: {}", elapsed);

    logDebug("layout tree: {}", result.layout);
    logDebug("paint tree: {}", result.paint);

    co_return Ok();
}
