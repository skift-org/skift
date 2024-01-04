#pragma once

// https://cachemon.github.io/SIEVE-website/
// https://github.com/scalalang2/golang-fifo/tree/main

#include <karm-base/list.h>

namespace Karm {

template <typename K, typename V>
struct Sieve {
    struct Node {
        K key;
        V value;
        bool visited = false;
        Node *prev = nullptr;
        Node *next = nullptr;
    };

    usize _cap;
    Ll<Node> _ll{};
    Node *_hand{};

    Sieve(usize cap) : _cap(cap) {}

    Node *_lookup(K const &key) {
        Node *node = _ll.head;
        while (node) {
            if (node->key == key)
                return node;
            node = node->next;
        }
        return nullptr;
    }

    void _evict() {
        auto *obj = _hand ?: _ll.tail;
        while (obj and obj->visited) {
            obj->visited = false;
            obj = obj->prev ?: _ll.tail;
        }
        _hand = obj->prev ?: _ll.tail;
        _ll.detach(obj);
        delete obj;
    }

    Opt<V> access(K const &key, auto const &make) {
        auto node = _lookup(key);
        if (node) {
            node->visited = true;
            return node->value;
        }

        if (_ll.len == _cap)
            _evict();

        auto value = make();
        _ll.prepend(new Node(key, value), _ll.head);
        return value;
    }

    Opt<V> get(K const &key) {
        auto node = _lookup(key);
        if (node) {
            node->visited = true;
            return node->value;
        }
        return NONE;
    }

    bool contains(K const &key) {
        return _lookup(key) != nullptr;
    }

    usize len() const {
        return _ll.len;
    }
};

} // namespace Karm
