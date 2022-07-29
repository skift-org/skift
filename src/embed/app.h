#pragma once

#include <karm-app/host.h>

namespace Embed {

Result<Strong<Karm::App::Host>> makeHost();

} // namespace Embed
