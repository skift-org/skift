#pragma once

#include "node.h"

namespace Web::Dom {

enum struct QuirkMode {
    NO,
    LIMITED,
    YES
};

// https://dom.spec.whatwg.org/#interface-document
struct Document : public Node {
    QuirkMode quirkMode{QuirkMode::NO};

    NodeType nodeType() const override {
        return NodeType::DOCUMENT;
    }
};

} // namespace Web::Dom
