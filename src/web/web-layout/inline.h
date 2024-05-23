#pragma once

#include "base.h"

namespace Web::Layout {

struct InlineFlow : public Flow {
    static constexpr auto TYPE = INLINE;

    struct Line {};

    Type type() const override {
        return TYPE;
    }
};

} // namespace Web::Layout
