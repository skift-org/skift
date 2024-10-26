#pragma once

#include <karm-math/flow.h>

#include "funcs.h"
#include "node.h"

namespace Karm::Ui {

// MARK: Scroll ----------------------------------------------------------------

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

// MARK: Clip ------------------------------------------------------------------

Child vhclip(Child child);

inline auto vhclip() {
    return [](Child child) {
        return vhclip(child);
    };
}

Child hclip(Child child);

inline auto hclip() {
    return [](Child child) {
        return hclip(child);
    };
}

Child vclip(Child child);

inline auto vclip() {
    return [](Child child) {
        return vclip(child);
    };
}

} // namespace Karm::Ui
