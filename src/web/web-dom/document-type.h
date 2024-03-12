#pragma once

#include "node.h"

namespace Web::Dom {

struct DocumentType : public Node {
    String name;
    String publicId;
    String systemId;

    DocumentType(String name, String publicId, String systemId)
        : name(name), publicId(publicId), systemId(systemId) {
    }

    virtual NodeType nodeType() override {
        return NodeType::DOCUMENT_TYPE_NODE;
    }
};

} // namespace Web::Dom
