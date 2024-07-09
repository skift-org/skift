#pragma once

#include <karm-mime/url.h>

namespace Karm::Pkg::_Embed {

Res<Vec<String>> installedBundles();

Res<String> currentBundle();

} // namespace Karm::Pkg::_Embed
