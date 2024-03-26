#pragma once

#include <karm-base/map.h>

#include "attr.h"
#include "node.h"

namespace Web::Dom {

// https://dom.spec.whatwg.org/#interface-element
struct Element : public Node {
    String tagName;
    // NOSPEC: Should be a NamedNodeMap
    Map<String, Strong<Attr>> attributes;

    Element(String tagName)
        : tagName(tagName) {
    }

    NodeType nodeType() const override {
        return NodeType::ELEMENT;
    }

    void _dump(Io::Emit &e) const override {
        e(" tagName={#}", this->tagName);
        for (auto const &[name, attr] : this->attributes.iter()) {
            e(" attr={#}", name);
            attr->_dump(e);
        }
    }
};

} // namespace Web::Dom
