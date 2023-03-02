#pragma once

#include <karm-layout/flow.h>

#include "funcs.h"
#include "node.h"

namespace Karm::Ui {

Child vhscroll(Child child);

inline auto vhscroll() {
    return [](Child child) {
        return vhscroll(child);
    };
}

Child hscroll(Child child);

inline auto hscroll() {
    return [](Child child) {
        return hscroll(child);
    };
}

Child vscroll(Child child);

inline auto vscroll() {
    return [](Child child) {
        return vscroll(child);
    };
}

using Build = Func<Child()>;

using BuildItem = Func<Child(usize)>;

Child hlist(usize len, BuildItem child);

Child vlist(usize len, BuildItem child);

} // namespace Karm::Ui
