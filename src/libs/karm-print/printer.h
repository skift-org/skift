#pragma once

#include <karm-gfx/canvas.h>
#include <karm-pdf/canvas.h>

#include "paper.h"

namespace Karm::Print {

struct Printer {
    virtual ~Printer() = default;

    virtual Gfx::Canvas &beginPage(PaperStock paper) = 0;
};

} // namespace Karm::Print
