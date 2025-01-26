#include <karm-sys/file.h>

#include "file-printer.h"
#include "image-printer.h"
#include "pdf-printer.h"

namespace Karm::Print {

Res<Rc<FilePrinter>> FilePrinter::create(Mime::Uti uti, FilePrinterProps props) {
    if (uti == Mime::Uti::PUBLIC_PDF) {
        return Ok(makeRc<PdfPrinter>());
    } else if (uti == Mime::Uti::PUBLIC_BMP or
               uti == Mime::Uti::PUBLIC_TGA or
               uti == Mime::Uti::PUBLIC_QOI) {
        return Ok(makeRc<ImagePrinter>(props.density, Image::Saver{uti}));
    }

    return Error::invalidData("cannot create printer");
}

Res<> FilePrinter::save(Mime::Url url) {
    auto outFile = try$(Sys::File::create(url));
    Io::TextEncoder<> outEncoder{outFile};
    try$(write(outEncoder));
    try$(outFile.flush());
    return Ok();
}

} // namespace Karm::Print
