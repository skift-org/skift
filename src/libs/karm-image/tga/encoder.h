#pragma once

#include <karm-gfx/buffer.h>
#include <karm-io/bscan.h>

#include "base.h"

namespace Tga {

Res<> encode(Gfx::Pixels pixels, Io::BEmit& e);

} // namespace Tga
