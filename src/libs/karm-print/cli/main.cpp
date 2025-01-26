#include <karm-print/page.h>
#include <karm-print/pdf-printer.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>

Async::Task<> entryPointAsync(Sys::Context&) {
    auto printer = co_try$(Print::PdfPrinter::create(Mime::Uti::PUBLIC_PDF));

    auto& ctx = printer->beginPage(Print::A4);
    ctx.fillStyle(Gfx::RED);
    ctx.rect({0, 0, 100, 100});
    ctx.fill(Gfx::FillRule::NONZERO);

    ctx.fillStyle(Gfx::BLUE);
    ctx.rect({0, 100, 100, 100});
    ctx.fill(Gfx::FillRule::NONZERO);

    ctx.fillStyle(Gfx::GREEN);
    ctx.rect({0, 200, 100, 100});
    ctx.fill(Gfx::FillRule::NONZERO);

    Text::Prose prose = Text::ProseStyle{
        .font = {
            co_try$(Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Regular.ttf"_url)),
            12
        },
        .color = Gfx::BLACK,
    };

    prose.append("Hello, world!"s);
    prose.layout(999);
    ctx.fill(prose);

    co_try$(printer->save("file:test.pdf"_url));

    co_return Ok();
}
