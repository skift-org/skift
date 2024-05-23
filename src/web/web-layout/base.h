#pragma once

#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <web-unit/length.h>

namespace Web::Layout {

struct Frag {
    Unit::RectPx borderBox;
    Unit::SpacingPx BorderPadding;
    Unit::SpacingPx margin;

    virtual ~Frag() = default;
};

struct Flow : public Frag {
    Vec<Strong<Frag>> children;
};

} // namespace Web::Layout
