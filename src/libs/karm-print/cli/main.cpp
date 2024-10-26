#include <karm-print/pdf.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>

Async::Task<> entryPointAsync(Sys::Context &) {
    Print::PdfPrinter printer{Print::A4};
    auto &ctx = printer.beginPage();
    ctx.fillStyle(Gfx::RED);
    ctx.rect({0, 0, 100, 100});
    ctx.fill(Gfx::FillRule::NONZERO);

    ctx.fillStyle(Gfx::BLUE);
    ctx.rect({0, 100, 100, 100});
    ctx.fill(Gfx::FillRule::NONZERO);

    ctx.fillStyle(Gfx::GREEN);
    ctx.rect({0, 200, 100, 100});
    ctx.fill(Gfx::FillRule::NONZERO);

    auto outFile = co_try$(Sys::File::create("file:test.pdf"_url));
    Io::TextEncoder<> outEncoder{outFile};
    printer.write(outEncoder);
    co_try$(outFile.flush());

    co_return Ok();
}
