#pragma once

#include "node.h"
#include "tags.h"

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

    void _repr(Io::Emit& e) const override {
        e(" localName={}:{} value={#}", name.ns.name(), name.name(), value);
    }
};

} // namespace Vaev::Dom
