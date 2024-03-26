#pragma once

#include "node.h"

namespace Web::Dom {

struct Element : public Node {
    String tagName;

    Element(String tagName)
        : tagName(tagName) {
    }

    NodeType nodeType() override {
        return NodeType::ELEMENT;
    }

    void _dump(Io::Emit &e) override {
        e(" tagName={#}", this->tagName);
    }
};

} // namespace Web::Dom
