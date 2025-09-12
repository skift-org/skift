export module Vaev.Engine:dom.element;

import Karm.Core;
import :dom.attr;
import :dom.node;
import :dom.names;
import :dom.text;
import :dom.token_list;

using namespace Karm;

namespace Vaev::Style {
export struct SpecifiedValues;

} // namespace Vaev::Style

namespace Vaev::Dom {

export struct PseudoElement {
    Opt<Rc<Style::SpecifiedValues>> _specifiedValues = NONE;

    Rc<Style::SpecifiedValues> specifiedValues() const {
        return _specifiedValues.unwrap("unstyled pseudo-element");
    }
};

// https://dom.spec.whatwg.org/#interface-element
export struct Element : Node {
    static constexpr auto TYPE = NodeType::ELEMENT;

    QualifiedName qualifiedName;
    // NOSPEC: Should be a NamedNodeMap
    Map<QualifiedName, Rc<Attr>> attributes;
    Opt<Rc<Style::SpecifiedValues>> _specifiedValues; // FIXME: We should not have this store here
    TokenList classList;

    Element(QualifiedName const& qualifiedName)
        : qualifiedName(qualifiedName) {
    }

    Symbol namespaceUri() const {
        return qualifiedName.ns;
    }

    Symbol localName() const {
        return qualifiedName.name;
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    Rc<Style::SpecifiedValues> specifiedValues() const {
        return _specifiedValues.unwrap("unstyled element");
    }

    Opt<Str> id() const {
        return this->getAttributeUnqualified("id"_sym);
    }

    Opt<Str> style() const {
        return this->getAttributeUnqualified("style"_sym);
    }

    // https://dom.spec.whatwg.org/#concept-descendant-text-content
    String textContent() {
        StringBuilder sb;
        for (auto child : iterDepthFirst()) {
            if (auto text = child->is<Text>())
                sb.append(text->data());
        }
        return sb.take();
    }

    void _repr(Io::Emit& e) const override {
        e(" qualifiedName={}", qualifiedName);
        if (this->attributes.len()) {
            e.indentNewline();
            for (auto const& [name, attr] : this->attributes.iter()) {
                attr->repr(e);
            }
            e.deindent();
        }
    }

    void setAttribute(QualifiedName name, String value) {
        if (name == Html::CLASS_ATTR or name == Svg::CLASS_ATTR) {
            for (auto class_ : iterSplit(value, ' ')) {
                this->classList.add(class_);
            }
            return;
        }
        this->attributes.put(name, makeRc<Attr>(name, value));
    }

    bool hasAttribute(QualifiedName name) const {
        return this->attributes.tryGet(name) != NONE;
    }

    bool hasAttributeUnqualified(Str name) const {
        for (auto const& [qualifiedName, _] : this->attributes.iter()) {
            if (qualifiedName.name.str() == name) {
                return true;
            }
        }
        return false;
    }

    Opt<Str> getAttribute(QualifiedName name) const {
        auto attr = this->attributes.tryGet(name);
        if (attr == NONE)
            return NONE;
        return (*attr)->value;
    }

    Opt<Str> getAttributeUnqualified(Symbol name) const {
        for (auto const& [qualifiedName, attr] : this->attributes.iter())
            if (qualifiedName.name == name)
                return attr->value;
        return NONE;
    }
};

} // namespace Vaev::Dom
