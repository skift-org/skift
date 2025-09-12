export module Vaev.Engine:dom.node;

import Karm.Ref;
import Karm.Gc;
import :dom.tree;

namespace Vaev::Dom {

export struct Document;

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

export enum struct NodeType {
#define ITER(NAME, VALUE) NAME = VALUE,
    FOREACH_NODE_TYPE(ITER)
#undef ITER
        _LEN,
};

// https://dom.spec.whatwg.org/#interface-node
export struct Node : Tree<Node> {
    virtual ~Node() = default;
    virtual NodeType nodeType() const = 0;

    template <typename T>
    Gc::Ptr<T> is() {
        if (nodeType() != T::TYPE)
            return nullptr;
        return {MOVE, static_cast<T*>(this)};
    }

    template <typename T>
    Gc::Ptr<T const> is() const {
        if (nodeType() != T::TYPE)
            return nullptr;
        return {MOVE, static_cast<T const*>(this)};
    }

    Ref::Url baseURI();

    Gc::Ptr<Document> ownerDocument();

    virtual void _repr(Io::Emit&) const {}

    void repr(Io::Emit& e) const;

    bool operator==(Node const& other) const {
        return this == &other;
    }

    auto operator<=>(Node const& other) const {
        return this <=> &other;
    }
};

} // namespace Vaev::Dom
