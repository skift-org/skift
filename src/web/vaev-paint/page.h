#pragma once

#include "stack.h"

namespace Vaev::Paint {

struct Page : public Stack {
    void print(Print::Printer &doc) override {
        Stack::print(doc);
        paint(doc.beginPage());
    }
};

} // namespace Vaev::Paint
