#pragma once

#include <karm-base/std.h>

namespace Karm::Gfx {

extern uint8_t const VGA_FONT[128 * 8];
[[gnu::used]] static constexpr int VGA_FONT_WIDTH = 8;
[[gnu::used]] static constexpr int VGA_FONT_HEIGHT = 8;

} // namespace Karm::Gfx
