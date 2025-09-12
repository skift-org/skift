export module Karm.Print:filePrinter;

import Karm.Ref;
import Karm.Core;
import :printer;

namespace Karm::Print {

export struct FilePrinterProps {
    /// Pixel density for raster formats (ignored for vector formats)
    f64 density = 1;
};

export struct FilePrinter : Printer {
    static Res<Rc<FilePrinter>> create(Ref::Uti uti, FilePrinterProps props = {});

    virtual Res<> write(Io::Writer& w) = 0;

    Res<> save(Ref::Url url);
};

} // namespace Karm::Print
