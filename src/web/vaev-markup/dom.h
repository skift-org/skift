#pragma once

#include <karm-base/list.h>
#include <karm-base/map.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-io/emit.h>

#include "tags.h"

namespace Vaev::Markup {

// MARK: Node ------------------------------------------------------------------

#define FOREACH_NODE_TYPE(TYPE)     \
    TYPE(ELEMENT, 1)                \
    TYPE(ATTRIBUTE, 2)              \
    TYPE(TEXT, 3)                   \
    TYPE(CDATA_SECTION, 4)          \
    TYPE(PROCESSING_INSTRUCTION, 7) \
    TYPE(COMMENT, 8)                \
    TYPE(DOCUMENT, 9)               \
    TYPE(DOCUMENT_TYPE, 10)         \
    TYPE(DOCUMENT_FRAGMENT, 11)

enum struct NodeType {
#define ITER(NAME, VALUE) NAME = VALUE,
    FOREACH_NODE_TYPE(ITER)
#undef ITER
        _LEN,
};

// https://dom.spec.whatwg.org/#interface-node
struct Node :
    Meta::Static {

    Node *_parent = nullptr;
    Vec<Strong<Node>> _children;
 
    virtual ~Node() = default;

    virtual NodeType nodeType() const = 0;

    template <typename T>
    T *is() {
        return nodeType() == T::TYPE ? static_cast<T *>(this) : nullptr;
    }

    template <typename T>
    T const *is() const {
        return nodeType() == T::TYPE ? static_cast<T const *>(this) : nullptr;
    }

    // MARK: Parent

    Node &parentNode() {
        if (not _parent)
            panic("node has no parent");
        return *_parent;
    }

    Node const &parentNode() const {
        if (not _parent)
            panic("node has no parent");
        return *_parent;
    }

    bool hasParent() const {
        return _parent != nullptr;
    }

    usize _parentIndex() const {
        if (not _parent)
            panic("node has no parent");
        return indexOf(parentNode()._children, *this).unwrap();
    }

    void _detachParent() {
        if (_parent) {
            _parent->_children.removeAt(_parentIndex());
            _parent = nullptr;
        }
    }

    // MARK: Children

    bool hasChildren() const {
        return _children.len() > 0;
    }

    Strong<Node> firstChild() {
        if (not _children.len())
            panic("node has no children");
        return first(_children);
    }

    Strong<Node> lastChild() {
        if (not _children.len())
            panic("node has no children");
        return last(_children);
    }

    void appendChild(Strong<Node> child) {
        child->_detachParent();
        _children.pushBack(child);
        child->_parent = this;
    }

    void removeChild(Strong<Node> child) {
        if (child->_parent != this)
            panic("node is not a child");
        child->_detachParent();
    }

    Slice<Strong<Node>> children() const {
        return _children;
    }

    // MARK: Siblings

    Strong<Node> previousSibling() const {
        usize index = _parentIndex();
        return parentNode()._children[index - 1];
    }

    bool hasPreviousSibling() const {
        return _parentIndex() > 0;
    }

    Strong<Node> nextSibling() {
        usize index = _parentIndex();
        return parentNode()._children[index + 1];
    }

    bool hasNextSibling() const {
        return _parentIndex() < parentNode()._children.len() - 1;
    }

    virtual void _repr(Io::Emit &) const {}

    void repr(Io::Emit &e) const {
        e("({}", nodeType());
        _repr(e);
        if (_children.len() > 0) {
            e.indentNewline();
            for (auto &child : _children) {
                child->repr(e);
            }
            e.deindent();
        }
        e(")\n");
    }

    // MARK: Operators

    bool operator==(Node const &other) const {
        return this == &other;
    }

    auto operator<=>(Node const &other) const {
        return this <=> &other;
    }
};

// MARK: Document --------------------------------------------------------------

enum struct QuirkMode {
    NO,
    LIMITED,
    YES
};

// https://dom.spec.whatwg.org/#interface-document
struct Document : public Node {
    static constexpr auto TYPE = NodeType::DOCUMENT;

    QuirkMode quirkMode{QuirkMode::NO};

    NodeType nodeType() const override {
        return TYPE;
    }
};

// MARK: DocumentType ----------------------------------------------------------

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

    void _repr(Io::Emit &e) const override {
        e(" name={#} publicId={#} systemId={#}", this->name, this->publicId, this->systemId);
    }
};

// MARK: CharacterData ---------------------------------------------------------

// https://dom.spec.whatwg.org/#interface-characterdata
struct CharacterData : public Node {
    String data;

    CharacterData(String data)
        : data(data) {
    }

    void appendData(String const &data) {
        // HACK: This is not efficient and pretty slow,
        //       but it's good enough for now.
        StringBuilder sb;
        sb.append(this->data);
        sb.append(data);
        this->data = sb.take();
    }

    void appendData(Rune rune) {
        // HACK: This is not efficient and pretty slow,
        //       but it's good enough for now.
        StringBuilder sb;
        sb.append(this->data);
        sb.append(rune);
        this->data = sb.take();
    }

    void _repr(Io::Emit &e) const override {
        e(" data={#}", this->data);
    }
};

// MARK: Text ------------------------------------------------------------------

// https://dom.spec.whatwg.org/#text
struct Text : public CharacterData {
    static constexpr auto TYPE = NodeType::TEXT;

    using CharacterData::CharacterData;

    NodeType nodeType() const override {
        return TYPE;
    }
};

// MARK: Attrs -----------------------------------------------------------------

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

// MARK: TokenList ------------------------------------------------------------

// https://dom.spec.whatwg.org/#domtokenlist
struct TokenList {
    Vec<String> _tokens;

    usize length() const {
        return _tokens.len();
    }

    Opt<String> item(usize index) const {
        if (index >= _tokens.len())
            return NONE;
        return _tokens[index];
    }

    bool contains(Str token) const {
        return _tokens.contains(token);
    }

    void add(Str token) {
        if (not _tokens.contains(token))
            _tokens.pushBack(token);
    }

    void remove(Str token) {
        _tokens.removeAll(token);
    }

    bool toggle(Str token) {
        if (_tokens.contains(token)) {
            _tokens.removeAll(token);
            return false;
        }
        _tokens.pushBack(token);
        return true;
    }

    bool replace(Str oldToken, Str newToken) {
        if (not _tokens.contains(oldToken))
            return false;
        _tokens.removeAll(oldToken);
        _tokens.pushBack(newToken);
        return true;
    }
};

// MARK: Element ---------------------------------------------------------------

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

// MARK: Comment ---------------------------------------------------------------

// https://dom.spec.whatwg.org/#interface-comment
struct Comment : public CharacterData {
    using CharacterData::CharacterData;

    static constexpr auto TYPE = NodeType::COMMENT;

    NodeType nodeType() const override {
        return TYPE;
    }
};

} // namespace Vaev::Markup
