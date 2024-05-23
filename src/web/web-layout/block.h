#pragma once

#include "base.h"

namespace Web::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    Type type() const override {
        return TYPE;
    }
};

} // namespace Web::Layout
