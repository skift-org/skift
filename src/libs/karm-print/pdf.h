#pragma once

#include <karm-pdf/canvas.h>
#include <karm-pdf/values.h>

#include "base.h"

namespace Karm::Print {

struct PdfPrinter : public Printer {
    PaperStock _stock;
    Vec<Io::StringWriter> _pages;
    Opt<Pdf::Canvas> _canvas;

    Gfx::Canvas &beginPage() override {
        _pages.emplaceBack();
        _canvas = Pdf::Canvas{last(_pages)};
        return *_canvas;
    }

    Pdf::File pdf() {
        Pdf::Ref alloc;

        Pdf::File file;
        file.header = "PDF-1.7"s;

        Pdf::Array pagesKids;
        Pdf::Ref pagesRef = alloc.alloc();

        // Page
        for (auto &p : _pages) {
            pagesKids.pushBack(file.add(
                alloc.alloc(),
                Pdf::Dict{
                    {"Type"s, Pdf::Name{"Page"s}},
                    {"Parent"s, pagesRef},
                    {
                        "Contents"s,
                        Pdf::Stream{
                            .dict = Pdf::Dict{
                                {"Length"s, p.bytes().len()},
                            },
                            .data = p.bytes(),
                        },
                    },
                }
            ));
        }

        // Pages
        file.add(
            pagesRef,
            Pdf::Dict{
                {"Type"s, Pdf::Name{"Pages"s}},
                {"MediaBox"s, Pdf::Array{usize{0}, usize{0}, _stock.width, _stock.height}},
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
};

} // namespace Karm::Print
