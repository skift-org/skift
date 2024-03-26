#pragma once

#include "node.h"

namespace Web::Dom {

enum struct QuirkMode {
    NO,
    LIMITED,
    YES
};

struct Document : public Node {
    QuirkMode quirkMode{QuirkMode::NO};

    NodeType nodeType() override {
        return NodeType::DOCUMENT;
    }
};

} // namespace Web::Dom
