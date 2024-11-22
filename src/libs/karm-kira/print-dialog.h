#pragma once

#include <karm-scene/page.h>
#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

using PrintPreview = SharedFunc<Vec<Strong<Scene::Page>>(Print::Settings const &)>;

Ui::Child printDialog(PrintPreview preview);

} // namespace Karm::Kira
