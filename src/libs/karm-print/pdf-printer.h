#pragma once

#include <karm-pdf/canvas.h>
#include <karm-pdf/values.h>

#include "file-printer.h"

namespace Karm::Print {

struct PdfPage {
    PaperStock paper;
    Io::StringWriter data;
};

struct PdfPrinter : public FilePrinter {
    Vec<PdfPage> _pages;
    Opt<Pdf::Canvas> _canvas;

    Gfx::Canvas& beginPage(PaperStock paper) override {
        auto& page = _pages.emplaceBack(paper);
        _canvas = Pdf::Canvas{page.data, paper.size()};

        // NOTE: PDF has the coordinate system origin at the bottom left corner.
        //       But we want to have it at the top left corner.
        _canvas->transform(
            {1, 0, 0, -1, 0, paper.height}
        );

        return *_canvas;
    }

    Pdf::File pdf() {
        Pdf::Ref alloc;

        Pdf::File file;
        file.header = "PDF-2.0"s;

        Pdf::Array pagesKids;
        Pdf::Ref pagesRef = alloc.alloc();

        // Page
        for (auto& p : _pages) {
            Pdf::Ref pageRef = alloc.alloc();
            Pdf::Ref contentsRef = alloc.alloc();

            file.add(
                pageRef,
                Pdf::Dict{
                    {"Type"s, Pdf::Name{"Page"s}},
                    {"Parent"s, pagesRef},
                    {"MediaBox"s,
                     Pdf::Array{
                         usize{0},
                         usize{0},
                         p.paper.width,
                         p.paper.height,
                     }},
                    {
                        "Contents"s,
                        contentsRef,
                    },
                }
            );

            file.add(
                contentsRef,
                Pdf::Stream{
                    .dict = Pdf::Dict{
                        {"Length"s, p.data.bytes().len()},
                    },
                    .data = p.data.bytes(),
                }
            );

            pagesKids.pushBack(pageRef);
        }

        // Pages
        file.add(
            pagesRef,
            Pdf::Dict{
                {"Type"s, Pdf::Name{"Pages"s}},
                {"Count"s, _pages.len()},
                {"Kids"s, std::move(pagesKids)},
            }
        );

        // Catalog
        auto catalogRef = file.add(
            alloc.alloc(),
            Pdf::Dict{
                {"Type"s, Pdf::Name{"Catalog"s}},
                {"Pages"s, pagesRef},
            }
        );

        // Trailer
        file.trailer = Pdf::Dict{
            {"Size"s, file.body.len() + 1},
            {"Root"s, catalogRef},
        };

        return file;
    }

    void write(Io::Emit& e) {
        pdf().write(e);
    }

    Res<> write(Io::Writer& w) override {
        Io::TextEncoder<> encoder{w};
        Io::Emit e{encoder};
        write(e);
        try$(e.flush());
        return Ok();
    }
};

} // namespace Karm::Print
