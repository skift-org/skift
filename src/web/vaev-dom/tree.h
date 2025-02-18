#pragma once

#include <karm-base/box.h>
#include <karm-gc/ptr.h>
#include <karm-meta/nocopy.h>

namespace Vaev::Dom {

enum struct Iter {
    CONTINUE,
    BREAK,
};

template <typename Node>
struct Tree : Meta::Pinned {
    Gc::Ptr<Node> _parent = nullptr;
    Gc::Ptr<Node> _firstChild = nullptr;
    Gc::Ptr<Node> _lastChild = nullptr;
    Gc::Ptr<Node> _nextSibling = nullptr;
    Gc::Ptr<Node> _prevSibling = nullptr;

    // Accessor ----------------------------------------------------------------

    usize index() const {
        usize index = 0;
        for (auto node = previousSibling();
             node;
             node = node->previousSibling())
            ++index;
        return index;
    }

    bool hasParentNode() const { return _parent != nullptr; }

    Gc::Ptr<Node> parentNode() const { return _parent; }

    bool hasChildren() const { return _firstChild != nullptr; }

    usize countChildren() const {
        usize count = 0;
        for (auto child = firstChild(); child; child = child->nextSibling())
            ++count;
        return count;
    }

    Gc::Ptr<Node> firstChild() const { return _firstChild; }

    Gc::Ptr<Node> lastChild() const { return _lastChild; }

    bool hasPreviousSibling() const { return _prevSibling != nullptr; }

    Gc::Ptr<Node> previousSibling() const { return _prevSibling; }

    bool hasNextSibling() const { return _nextSibling != nullptr; }

    Gc::Ptr<Node> nextSibling() const { return _nextSibling; }

    // Insertion & Deletion ----------------------------------------------------

    void appendChild(Gc::Ptr<Node> node) {
        if (node->_parent)
            panic("node already has a parent");

        if (_lastChild)
            _lastChild->_nextSibling = node;
        node->_prevSibling = _lastChild;
        node->_parent = {MOVE, static_cast<Node*>(this)};
        _lastChild = node;
        if (!_firstChild)
            _firstChild = _lastChild;
    }

    void prependChild(Gc::Ptr<Node> node) {
        if (node->_parent)
            panic("node already has a parent");

        if (_firstChild)
            _firstChild->_prevSibling = node;
        node->_nextSibling = _firstChild;
        node->_parent = static_cast<Node*>(this);
        _firstChild = node;
        if (!_lastChild)
            _lastChild = _firstChild;
    }

    void insertBefore(Node* node, Node* child) {
        if (!child)
            return appendChild(node);

        if (child->_parent != this)
            panic("node is not a child");

        if (node->_parent)
            panic("node already has a parent");

        node->_prevSibling = child->_prevSibling;
        node->_nextSibling = child;

        if (child->_prevSibling)
            child->_prevSibling->_nextSibling = node;

        if (_firstChild == child)
            _firstChild = node;

        child->_prevSibling = node;

        node->_parent = static_cast<Node*>(this);
    }

    void insertAfter(Node* node, Node* child) {
        if (child->_parent != this)
            panic("node is not a child");

        if (node->_parent)
            panic("node already has a parent");

        node->_prevSibling = child;
        node->_nextSibling = child->_nextSibling;

        if (child->_nextSibling)
            child->_nextSibling->_prevSibling = node;

        if (_lastChild == child)
            _lastChild = node;

        child->_nextSibling = node;

        node->_parent = static_cast<Node*>(this);
    }

    void removeChild(Node* node) {
        if (node->_parent != this)
            panic("node is not a child");

        if (node->_parent)
            panic("node already has a parent");

        if (_firstChild == node)
            _firstChild = node->_nextSibling;

        if (_lastChild == node)
            _lastChild = node->_prevSibling;

        if (node->_nextSibling)
            node->_nextSibling->_prevSibling = node->_prevSibling;

        if (node->_prevSibling)
            node->_prevSibling->_nextSibling = node->_nextSibling;

        node->_nextSibling = nullptr;
        node->_prevSibling = nullptr;
        node->_parent = nullptr;
    }

    // Iteration ---------------------------------------------------------------

    Iter iterDepthFirst(this auto& self, auto f) {
        if (f(self) == Iter::BREAK)
            return Iter::BREAK;

        for (auto child = self.firstChild(); child; child = child->nextSibling())
            if (child->iterDepthFirst(f) == Iter::BREAK)
                return Iter::BREAK;
        return Iter::CONTINUE;
    }
};

} // namespace Vaev::Dom
