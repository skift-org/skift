#pragma once

#include <karm-base/map.h>
#include <karm-base/slice.h>
#include <vaev-base/tags.h>
#include <vaev-dom/token-list.h>
#include <vaev-html/tags.h>

#include "attr.h"
#include "node.h"
#include "text.h"

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-element
struct Element : public Node {
    static constexpr auto TYPE = NodeType::ELEMENT;

    Opt<String> id() const {
        return this->getAttribute(Html::ID_ATTR);
    }

    TagName tagName;
    // NOSPEC: Should be a NamedNodeMap
    Map<AttrName, Strong<Attr>> attributes;
    TokenList classList;

    Element(TagName tagName)
        : tagName(tagName) {
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    String textContent() const {
        String builder;
        if (_children.len() == 0)
            return ""s;

        if (_children.len() > 1)
            panic("textContent is not implemented for elements with multiple children");

        auto const &child = *_children[0];
        if (auto *text = child.is<Text>()) {
            return text->data;
        }

        panic("textContent is not implemented for elements with children other than text nodes");
    }

    void _repr(Io::Emit &e) const override {
        e(" tagName={#}", this->tagName);
        if (this->attributes.len()) {
            e.indentNewline();
            for (auto const &[name, attr] : this->attributes.iter()) {
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
        auto attr = makeStrong<Attr>(name, value);
        this->attributes.put(name, attr);
    }

    bool hasAttribute(AttrName name) const {
        return this->attributes.tryGet(name) != NONE;
    }

    Opt<String> getAttribute(AttrName name) const {
        auto attr = this->attributes.tryGet(name);
        if (attr == NONE)
            return NONE;
        return (*attr)->value;
    }
};

} // namespace Vaev::Dom
