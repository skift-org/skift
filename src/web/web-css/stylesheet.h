#pragma once

#include <karm-base/rc.h>
#include <karm-base/vec.h>

namespace Web::Css {

struct Rule;

struct StyleSheet {
    Vec<Strong<Rule>> cssRules;

    StyleSheet() = default;
};

} // namespace Web::Css
