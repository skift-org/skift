#pragma once

#include <karm-pdf/canvas.h>
#include <karm-pdf/values.h>

#include "file-printer.h"

namespace Karm::Print {

struct PdfPrinter : public FilePrinter {
    PaperStock _paper;
    Vec<Io::StringWriter> _pages;
    Opt<Pdf::Canvas> _canvas;

    PdfPrinter(PaperStock stock)
        : _paper(stock) {}

    Gfx::Canvas &beginPage() override {
        _pages.emplaceBack();
        _canvas = Pdf::Canvas{last(_pages), _paper.size()};
        return *_canvas;
    }

    Pdf::File pdf() {
        Pdf::Ref alloc;

        Pdf::File file;
        file.header = "PDF-2.0"s;

        Pdf::Array pagesKids;
        Pdf::Ref pagesRef = alloc.alloc();

        // Page
        for (auto &p : _pages) {
            Pdf::Ref pageRef = alloc.alloc();
            Pdf::Ref contentsRef = alloc.alloc();

            file.add(
                pageRef,
                Pdf::Dict{
                    {"Type"s, Pdf::Name{"Page"s}},
                    {"Parent"s, pagesRef},
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
                        {"Length"s, p.bytes().len()},
                    },
                    .data = p.bytes(),
                }
            );

            pagesKids.pushBack(pageRef);
        }

        // Pages
        file.add(
            pagesRef,
            Pdf::Dict{
                {"Type"s, Pdf::Name{"Pages"s}},
                {"MediaBox"s,
                 Pdf::Array{
                     usize{0},
                     usize{0},
                     _paper.width,
                     _paper.height,
                 }},
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

    void write(Io::Emit &e) {
        pdf().write(e);
    }

    void write(Io::Writer &w) override {
        Io::TextEncoder<> encoder{w};
        Io::Emit e{encoder};
        write(e);
    }
};

} // namespace Karm::Print
