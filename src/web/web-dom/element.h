#pragma once

#include <karm-base/map.h>
#include <web-base/tags.h>

#include "attr.h"
#include "node.h"

namespace Web::Dom {

// https://dom.spec.whatwg.org/#interface-element
struct Element : public Node {
    TagName tagName;
    // NOSPEC: Should be a NamedNodeMap
    Map<AttrName, Strong<Attr>> attributes;

    Element(TagName tagName)
        : tagName(tagName) {
    }

    NodeType nodeType() const override {
        return NodeType::ELEMENT;
    }

    void _dump(Io::Emit &e) const override {
        e(" tagName={#}", this->tagName);
        if (this->attributes.len()) {
            e.indentNewline();
            for (auto const &[name, attr] : this->attributes.iter()) {
                attr->dump(e);
            }
            e.deindent();
        }
    }

    void setAttribute(AttrName name, String value) {
        auto attr = makeStrong<Attr>(name, value);
        this->attributes.put(name, attr);
    }
};

} // namespace Web::Dom
