#pragma once

#include "attr.h"
#include "node.h"
#include "tags.h"
#include "text.h"
#include "token-list.h"

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-element
struct Element : public Node {
    static constexpr auto TYPE = NodeType::ELEMENT;

    Opt<Str> id() const {
        return this->getAttribute(Html::ID_ATTR);
    }

    TagName tagName;
    // NOSPEC: Should be a NamedNodeMap
    Map<AttrName, Rc<Attr>> attributes;
    TokenList classList;

    Element(TagName tagName)
        : tagName(tagName) {
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    String textContent() const {
        String builder;
        if (not hasChildren())
            return ""s;

        if (firstChild() != lastChild())
            panic("textContent is not implemented for elements with multiple children");

        if (auto text = firstChild()->is<Text>())
            return text->data();

        panic("textContent is not implemented for elements with children other than text nodes");
    }

    void _repr(Io::Emit& e) const override {
        e(" tagName={#}", this->tagName);
        if (this->attributes.len()) {
            e.indentNewline();
            for (auto const& [name, attr] : this->attributes.iter()) {
                attr->repr(e);
            }
            e.deindent();
        }
    }

    void setAttribute(AttrName name, String value) {
        if (name == Html::CLASS_ATTR) {
            for (auto class_ : iterSplit(value, ' ')) {
                this->classList.add(class_);
            }
            return;
        }
        auto attr = makeRc<Attr>(name, value);
        this->attributes.put(name, attr);
    }

    bool hasAttribute(AttrName name) const {
        return this->attributes.tryGet(name) != NONE;
    }

    Opt<Str> getAttribute(AttrName name) const {
        auto attr = this->attributes.tryGet(name);
        if (attr == NONE)
            return NONE;
        return (*attr)->value;
    }
};

} // namespace Vaev::Dom
