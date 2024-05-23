#pragma once

#include "base.h"

namespace Web::Layout {

struct TextFrag : public Frag {
    static constexpr auto TYPE = TEXT;

    Type type() const override {
        return TYPE;
    }
};

} // namespace Web::Layout
