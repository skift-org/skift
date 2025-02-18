#pragma once

#include "node.h"

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-documenttype
struct DocumentType : public Node {
    static constexpr auto TYPE = NodeType::DOCUMENT_TYPE;

    String name;
    String publicId;
    String systemId;

    DocumentType() = default;

    DocumentType(String name, String publicId, String systemId)
        : name(name), publicId(publicId), systemId(systemId) {
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    void _repr(Io::Emit& e) const override {
        e(" name={#} publicId={#} systemId={#}", this->name, this->publicId, this->systemId);
    }
};

} // namespace Vaev::Dom
