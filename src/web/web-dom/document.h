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

    virtual NodeType nodeType() override {
        return NodeType::DOCUMENT_NODE;
    }
};

} // namespace Web::Dom
