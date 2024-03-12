#pragma once

#include <karm-base/list.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>

namespace Web::Dom {

enum struct NodeType {
    ELEMENT_NODE = 1,
    ATTRIBUTE_NODE = 2,
    TEXT_NODE = 3,
    CDATA_SECTION_NODE = 4,
    PROCESSING_INSTRUCTION_NODE = 7,
    COMMENT_NODE = 8,
    DOCUMENT_NODE = 9,
    DOCUMENT_TYPE_NODE = 10,
    DOCUMENT_FRAGMENT_NODE = 11,
};

struct Node : public Meta::Static {
    Node *_parent = nullptr;
    LlItem<Node> _siblings;
    Vec<Strong<Node>> _children;

    virtual ~Node() = default;

    virtual NodeType nodeType() = 0;

    /* --- Parent --- */

    Node &parentNode() {
        if (not _parent)
            panic("node has no parent");
        return *_parent;
    }

    usize _parentIndex() {
        return indexOf(parentNode()._children, *this).unwrap();
    }

    void _detachParent() {
        if (_parent) {
            _parent->_children.removeAt(_parentIndex());
            _parent = nullptr;
        }
    }

    /* --- Children --- */

    bool hasChildren() {
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

    /* --- Siblings --- */

    Strong<Node> previousSibling() {
        usize index = _parentIndex();
        return parentNode()._children[index - 1];
    }

    Strong<Node> nextSibling() {
        usize index = _parentIndex();
        return parentNode()._children[index + 1];
    }

    /* --- Operators --- */

    bool operator==(Node const &other) const {
        return this == &other;
    }

    auto operator<=>(Node const &other) const {
        return this <=> &other;
    }
};

} // namespace Web::Dom
