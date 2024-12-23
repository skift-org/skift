#pragma once

#include <karm-gfx/cpu/canvas.h>
#include <karm-image/saver.h>
#include <karm-print/file-printer.h>

namespace Karm::Print {

struct ImagePrinter : public FilePrinter {
    static constexpr isize GAPS = 16;

    Vec<Strong<Gfx::Surface>> _pages;
    Opt<Gfx::CpuCanvas> _canvas;
    f64 _density;
    Image::Saver _saver;

    ImagePrinter(f64 density = 1, Image::Saver saver = {})
        : _density(density),
          _saver(saver) {}

    Gfx::Canvas &beginPage(PaperStock paper) override {
        _pages.emplaceBack(Gfx::Surface::alloc(paper.size().cast<isize>() * _density, Gfx::RGBA8888));

        if (_canvas)
            _canvas->end();
        _canvas = Gfx::CpuCanvas{};
        _canvas->begin(*last(_pages));
        _canvas->scale(_density);
        _canvas->clear(Gfx::WHITE);

        return *_canvas;
    }

    Strong<Gfx::Surface> _mergedImages() {
        if (_pages.len() == 0)
            return Gfx::Surface::alloc(GAPS, Gfx::RGBA8888);

        isize finalHeight =
            iter(_pages)
                .map([](auto &page) {
                    return page->height() + GAPS;
                })
                .sum();
        finalHeight -= GAPS;

        isize finalWidth =
            iter(_pages)
                .map([](auto &page) {
                    return page->width();
                })
                .max()
                .unwrapOr(0);

        auto finalImageSize = Math::Vec2i{
            finalWidth,
            finalHeight,
        };

        auto finalImage = Gfx::Surface::alloc(finalImageSize, Gfx::RGBA8888);

        auto finalCanvas = Gfx::CpuCanvas{};
        finalCanvas.begin(*finalImage);
        finalCanvas.clear(Gfx::BLACK);

        isize ypos{0};
        for (auto &page : _pages) {
            finalCanvas.blit(
                page->bound(),
                page->bound().offset({0, ypos}),
                page->pixels()
            );
            ypos += page->height() + GAPS;
        }

        finalCanvas.end();

        return finalImage;
    }

    Res<> write(Io::Writer &w) override {
        return Image::save(
            _mergedImages()->pixels(),
            w,
            _saver
        );
    }
};

} // namespace Karm::Print
