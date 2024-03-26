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

    NodeType nodeType() override {
        return NodeType::DOCUMENT_TYPE;
    }

    void _dump(Io::Emit &e) override {
        e(" name={#} publicId={#} systemId={#}", this->name, this->publicId, this->systemId);
    }
};

} // namespace Web::Dom
