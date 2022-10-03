#pragma once

#include <karm-layout/flow.h>

#include "funcs.h"
#include "proxy.h"

namespace Karm::Ui {

Child hscroll(Child child);

Child vscroll(Child child);

using Build = Func<Child()>;
using BuildItem = Func<Child(size_t)>;

Child hlist(size_t count, BuildItem child);

Child vlist(size_t count, BuildItem child);

} // namespace Karm::Ui
