#pragma once

#include <karm-print/base.h>

namespace Karm::Print {

struct FilePrinter : public Printer {
    virtual ~FilePrinter() = default;

    virtual void write(Io::Writer &w) = 0;
};

} // namespace Karm::Print
