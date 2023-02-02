#pragma once

#include <karm-ui/host.h>

namespace Embed {

Result<Strong<Karm::Ui::Host>> makeHost(Karm::Ui::Child root);

} // namespace Embed
