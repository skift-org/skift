#include "font.h"

#include "font-vga.h"

namespace Karm::Media {

Strong<Font> Font::fallback() {
    return makeStrong<VgaFont>();
}

} // namespace Karm::Media
