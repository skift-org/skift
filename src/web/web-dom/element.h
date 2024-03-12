#pragma once

#include "node.h"

namespace Web::Dom {

struct Element : public Node {
    String tagName;

    Element(String tagName)
        : tagName(tagName) {
    }

    virtual NodeType nodeType() override {
        return NodeType::ELEMENT_NODE;
    }
};

} // namespace Web::Dom
