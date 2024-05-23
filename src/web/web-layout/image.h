#pragma once

#include "base.h"

namespace Web::Layout {

struct ImageFrag : public Frag {
    static constexpr auto TYPE = IMAGE;

    Type type() const override {
        return TYPE;
    }
};

} // namespace Web::Layout
