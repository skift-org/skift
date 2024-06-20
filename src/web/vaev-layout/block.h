#pragma once

#include "base.h"

namespace Vaev::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void layout(RectPx bound) override {
        Flow::layout(bound);
    }
};

} // namespace Vaev::Layout
