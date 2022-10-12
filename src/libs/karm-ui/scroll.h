#pragma once

#include <karm-layout/flow.h>

#include "funcs.h"
#include "proxy.h"

namespace Karm::Ui {

Child vhscroll(Child child);

Child hscroll(Child child);

Child vscroll(Child child);

using Build = Func<Child()>;

using BuildItem = Func<Child(size_t)>;

Child hlist(size_t len, BuildItem child);

Child vlist(size_t len, BuildItem child);

} // namespace Karm::Ui
