#pragma once

#include <karm-gfx/buffer.h>
#include <karm-io/bscan.h>

namespace Bmp {

Res<> encode(Gfx::Pixels pixels, Io::BEmit& e);

} // namespace Bmp
