#pragma once

#include "node.h"

namespace Vaev::Dom {

enum struct QuirkMode {
    NO,
    LIMITED,
    YES
};

// https://dom.spec.whatwg.org/#interface-document
struct Document : public Node {
    static constexpr auto TYPE = NodeType::DOCUMENT;

    QuirkMode quirkMode{QuirkMode::NO};

    NodeType nodeType() const override {
        return TYPE;
    }
};

} // namespace Vaev::Dom
