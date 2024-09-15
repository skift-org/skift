#pragma once

#include <karm-sys/context.h>

#include "node.h"

namespace Karm::Ui {

Child inspector(Child child);

Res<> runApp(Sys::Context &ctx, Child root);

} // namespace Karm::Ui
