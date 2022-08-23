#pragma once

#include "font.h"

namespace Karm::Media {

Result<Strong<Font>> loadFont(Str path);

} // namespace Karm::Media
