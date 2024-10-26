#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child sidePanelContent(Ui::Children children);

Ui::Child sidePanelTitle(Ui::OnPress onClose, Str title);

} // namespace Karm::Kira
