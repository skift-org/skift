#pragma once

#include <karm-base/list.h>

namespace Web::Dom {

struct Node : public Meta::Static {
    Node *_parent = nullptr;
    LlItem<Node> _siblings;
    Ll<Node, &Node::_siblings> _children;

    /* --- Parent --- */

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

    /* --- Children --- */

    bool hasChildren() const {
        return _children.len() > 0;
    }

    Node &firstChild() {
        auto *res = _children._head;
        if (not res)
            panic("node has no children");
        return *res;
    }

    Node const &firstChild() const {
        auto *res = _children._head;
        if (not res)
            panic("node has no children");
        return *res;
    }

    Node &lastChild() {
        auto *res = _children._tail;
        if (not res)
            panic("node has no children");
        return *res;
    }

    Node const &lastChild() const {
        auto *res = _children._tail;
        if (not res)
            panic("node has no children");
        return *res;
    }

    /* --- Siblings --- */

    Node &previousSibling() {
        auto *res = _siblings.prev;
        if (not res)
            panic("node has no previous sibling");
        return *res;
    }

    Node const &previousSibling() const {
        auto *res = _siblings.prev;
        if (not res)
            panic("node has no previous sibling");
        return *res;
    }

    Node &nextSibling() {
        auto *res = _siblings.next;
        if (not res)
            panic("node has no next sibling");
        return *res;
    }

    Node const &nextSibling() const {
        auto *res = _siblings.next;
        if (not res)
            panic("node has no next sibling");
        return *res;
    }
};

} // namespace Web::Dom
