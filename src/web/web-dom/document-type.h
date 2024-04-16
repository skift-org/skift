#pragma once

#include "node.h"

namespace Web::Dom {

// https://dom.spec.whatwg.org/#interface-documenttype
struct DocumentType : public Node {
    String name;
    String publicId;
    String systemId;

    DocumentType() = default;

    DocumentType(String name, String publicId, String systemId)
        : name(name), publicId(publicId), systemId(systemId) {
    }

    NodeType nodeType() const override {
        return NodeType::DOCUMENT_TYPE;
    }

    void _dump(Io::Emit &e) const override {
        e(" name={#} publicId={#} systemId={#}", this->name, this->publicId, this->systemId);
    }
};

} // namespace Web::Dom
