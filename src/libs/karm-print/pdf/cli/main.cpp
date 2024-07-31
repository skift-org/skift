#include <karm-sys/entry.h>
#include <karm-sys/file.h>

#include "../objects.h"

Async::Task<> entryPointAsync(Sys::Context &) {
    Pdf::Ref ref;

    Pdf::File file;

    file.header = "PDF-1.7"s;

    auto catalogRef = ref.alloc();
    auto pagesRef = ref.alloc();
    auto pageRef = ref.alloc();
    auto fontRef = ref.alloc();
    auto contentRef = ref.alloc();

    // Catalog

    Pdf::Dict catalog;
    catalog.put("Type"s, Pdf::Name{"Catalog"s});
    catalog.put("Pages"s, pagesRef);

    file.add(catalogRef, std::move(catalog));

    // Pages

    Pdf::Dict pages;
    pages.put("Type"s, Pdf::Name{"Pages"s});

    Pdf::Array mediaBox;
    mediaBox.pushBack(isize{0});
    mediaBox.pushBack(isize{0});
    mediaBox.pushBack(isize{200});
    mediaBox.pushBack(isize{200});

    pages.put("MediaBox"s, mediaBox);
    pages.put("Count"s, isize{1});
    pages.put("Kids"s, Pdf::Array{pageRef});

    file.add(pagesRef, std::move(pages));

    // Page

    Pdf::Dict page;
    page.put("Type"s, Pdf::Name{"Page"s});
    page.put("Parent"s, pagesRef);

    Pdf::Dict resources;
    Pdf::Dict fonts = {};
    fonts.put("F1"s, fontRef);
    resources.put("Font"s, fonts);
    page.put("Resources"s, resources);

    page.put("Contents"s, contentRef);

    file.add(pageRef, std::move(page));

    // Font

    Pdf::Dict font;
    font.put("Type"s, Pdf::Name{"Font"s});
    font.put("Subtype"s, Pdf::Name{"Type1"s});
    font.put("BaseFont"s, Pdf::Name{"Times-Roman"s});

    file.add(fontRef, std::move(font));

    // Content

    Io::BufferWriter pageStream;
    Io::BEmit pageEmit{pageStream};
    pageEmit.writeStr(
        R"(BT
70 50 TD
/F1 12 Tf
(Hello, world!) Tj
ET)"s
    );

    Pdf::Stream content;
    content.dict.put("Length"s, (isize)pageStream.bytes().len());
    content.data = pageStream.take();

    file.add(contentRef, std::move(content));

    file.trailer.put("Root"s, catalogRef);
    file.trailer.put("Size"s, (isize)file.body.len() + 1);

    Io::Emit out{Sys::out()};
    Pdf::write(out, file);
    co_try$(Sys::out().flush());

    co_return Ok();
}
