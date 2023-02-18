#pragma once

#include <karm-ui/host.h>

namespace Embed {

Res<Strong<Karm::Ui::Host>> makeHost(Karm::Ui::Child root);

} // namespace Embed
