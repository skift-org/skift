#pragma once

#include "flow.h"

namespace Vaev::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void placeChildren(Context &ctx, Box box) override;

    Px computeIntrinsicSize(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) override;
};

} // namespace Vaev::Layout
