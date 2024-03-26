#pragma once

#include "node.h"

namespace Web::Dom {

// https://dom.spec.whatwg.org/#interface-attr
struct Attr : public Node {
    Opt<String> namespaceURI;
    Opt<String> prefix;
    String localName;
    String value;

    NodeType nodeType() const override {
        return NodeType::ATTRIBUTE;
    }

    void _dump(Io::Emit &e) const override {
        if (namespaceURI)
            e(" namespaceURI={#}", namespaceURI);

        if (prefix)
            e(" prefix={#}", prefix);

        e(" localName={#} value={#}", localName, value);
    }
};

} // namespace Web::Dom
