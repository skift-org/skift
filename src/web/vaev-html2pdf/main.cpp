#include <karm-print/pdf.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>
#include <vaev-driver/fetcher.h>
#include <vaev-driver/render.h>

namespace Vaev {

Style::Media constructMedia(Print::PaperStock paper) {
    return {
        .type = MediaType::SCREEN,
        .width = Px{paper.width},
        .height = Px{paper.height},
        .aspectRatio = paper.width / paper.height,
        .orientation = Orientation::LANDSCAPE,

        .resolution = Resolution::fromDpi(96),
        .scan = Scan::PROGRESSIVE,
        .grid = false,
        .update = Update::NONE,

        .overflowBlock = OverflowBlock::PAGED,
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
    auto output = co_try$(Mime::parseUrlOrPath(args[1]));

    auto dom = co_try$(Vaev::Driver::fetchDocument(input));

    auto paper = Print::A4;
    auto media = Vaev::constructMedia(paper);

    auto start = Sys::now();
    auto [layout, paint] = Vaev::Driver::render(*dom, media, paper);
    auto elapsed = Sys::now() - start;
    logInfo("render time: {}", elapsed);

    logDebug("layout tree: {}", layout);
    logDebug("paint tree: {}", paint);

    Print::PdfPrinter printer;
    paint->print(printer);

    auto file = co_try$(Sys::File::create(output));
    Io::TextEncoder<> encoder{file};
    Io::Emit e{encoder};
    printer.write(e);
    co_try$(e.flush());

    co_return Ok();
}
