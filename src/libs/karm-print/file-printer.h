#pragma once

#include <karm-print/printer.h>

namespace Karm::Print {

struct FilePrinter : public Printer {
    virtual Res<> write(Io::Writer &w) = 0;
};

} // namespace Karm::Print
