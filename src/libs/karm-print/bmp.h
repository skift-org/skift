#pragma once

#include <karm-gfx/cpu/canvas.h>
#include <karm-image/saver.h>
#include <karm-print/file-printer.h>

namespace Karm::Print {

struct BMPPrinter : public FilePrinter {
    isize const VERTICAL_GAP_PX_SIZE = 16;
    PaperStock _paper;
    Vec<Strong<Gfx::Surface>> _pages;
    Opt<Gfx::CpuCanvas> _canvas;

    BMPPrinter(PaperStock stock)
        : _paper(stock) {}

    Gfx::Canvas &beginPage() override {
        _pages.emplaceBack(Gfx::Surface::alloc(_paper.size().cast<isize>(), Gfx::RGBA8888));

        if (_canvas)
            _canvas->end();
        _canvas = Gfx::CpuCanvas{};
        _canvas->begin(*last(_pages));
        _canvas->clear(Gfx::WHITE);

        return *_canvas;
    }

    Strong<Gfx::Surface> mergedImages() {
        isize finalHeight =
            _paper.size().cast<isize>().y * _pages.len() +
            VERTICAL_GAP_PX_SIZE * _pages.len();

        auto finalImageSize = Math::Vec2i{
            _paper.size().cast<isize>().x,
            finalHeight,
        };

        auto finalImage = Gfx::Surface::alloc(finalImageSize, Gfx::RGBA8888);

        auto finalCanvas = Gfx::CpuCanvas{};
        finalCanvas.begin(*finalImage);
        finalCanvas.clear(Gfx::BLACK);

        isize offsetY{0};
        for (auto &page : _pages) {
            finalCanvas.blit(
                page->bound(),
                page->bound().offset({0, offsetY}),
                page->pixels()
            );
            offsetY += page->height() + VERTICAL_GAP_PX_SIZE;
        }

        finalCanvas.end();

        return finalImage;
    }

    void write(Io::Writer &w) override {
        Karm::Image::save(mergedImages()->pixels(), w).take();
    }
};

} // namespace Karm::Print
