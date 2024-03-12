#pragma once

#include "node.h"

namespace Web::Dom {

struct Element : public Node {
    virtual NodeType nodeType() override {
        return NodeType::ELEMENT_NODE;
    }
};

} // namespace Web::Dom
