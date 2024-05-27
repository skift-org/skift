#pragma once

#include <karm-io/emit.h>
#include <karm-io/sscan.h>

#include "dom.h"

namespace Pdf {

Res<> parse(Io::SScan &s, Document &doc);

Res<> emit(Io::Emit &e, Document const &doc);

} // namespace Pdf
