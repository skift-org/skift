#pragma once

#include "stack.h"

namespace Web::Paint {

struct Page : public Stack {
    void print(Print::Context &ctx) override {
        for (auto &child : _children)
            child->print(ctx);
        ctx.pageBreak();
    }
};

} // namespace Web::Paint
