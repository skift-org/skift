module;

#include <karm-sys/file.h>

module Karm.Print;

import Karm.Image;
import Karm.Ref;

import :filePrinter;
import :imagePrinter;
import :pdfPrinter;

namespace Karm::Print {

Res<Rc<FilePrinter>> FilePrinter::create(Ref::Uti uti, FilePrinterProps props) {
    if (uti == Ref::Uti::PUBLIC_PDF) {
        return Ok(makeRc<PdfPrinter>());
    } else if (uti == Ref::Uti::PUBLIC_BMP or
               uti == Ref::Uti::PUBLIC_TGA or
               uti == Ref::Uti::PUBLIC_QOI) {
        return Ok(makeRc<ImagePrinter>(props.density, Image::Saver{uti}));
    }

    return Error::invalidData("cannot create printer");
}

Res<> FilePrinter::save(Ref::Url url) {
    auto outFile = try$(Sys::File::create(url));
    Io::TextEncoder<> outEncoder{outFile};
    try$(write(outEncoder));
    try$(outFile.flush());
    return Ok();
}

} // namespace Karm::Print
