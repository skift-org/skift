#pragma once

#include "base.h"

namespace Web::Layout {

struct TableFlow : public Flow {
    static constexpr auto TYPE = TABLE;

    Type type() const override {
        return TYPE;
    }
};

} // namespace Web::Layout
