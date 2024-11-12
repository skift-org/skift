#pragma once

#include <karm-gfx/buffer.h>
#include <karm-io/bscan.h>

namespace Qoi {

Res<> encode(Gfx::Pixels pixels, Io::BEmit &e);

} // namespace Qoi
