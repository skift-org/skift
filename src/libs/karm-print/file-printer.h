#pragma once

#include <karm-mime/url.h>
#include <karm-mime/uti.h>
#include <karm-print/printer.h>

namespace Karm::Print {

struct FilePrinterProps {
    /// Pixel density for raster formats (ignored for vector formats)
    f64 density = 1;
};

struct FilePrinter : public Printer {
    static Res<Rc<FilePrinter>> create(Mime::Uti uti, FilePrinterProps props = {});

    virtual Res<> write(Io::Writer& w) = 0;

    Res<> save(Mime::Url url);
};

} // namespace Karm::Print
