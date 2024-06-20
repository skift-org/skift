#pragma once

#include <vaev-base/tags.h>

#include "node.h"

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-attr
struct Attr : public Node {
    AttrName name;
    String value;

    Attr(AttrName name, String value)
        : name(name), value(value) {
    }

    NodeType nodeType() const override {
        return NodeType::ATTRIBUTE;
    }

    void _repr(Io::Emit &e) const override {
        e(" namespaceURI={#}", name.ns.url());
        e(" prefix={#}", name.ns.name());
        e(" localName={#} value={#}", name.name(), value);
    }
};

} // namespace Vaev::Dom
