#pragma once

#include <karm-gfx/buffer.h>
#include <karm-io/bscan.h>

namespace Jpeg {

Res<> encode(Gfx::Pixels pixels, Io::BEmit& e);

} // namespace Jpeg
