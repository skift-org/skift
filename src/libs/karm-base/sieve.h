#pragma once

// https://cachemon.github.io/SIEVE-website/
// https://github.com/scalalang2/golang-fifo/tree/main

#include "list.h"

namespace Karm {

template <typename K, typename V>
struct Sieve {
    struct Item {
        K key;
        V value;
        bool visited = false;
        LlItem<Item> item{};
    };

    usize _cap;
    Ll<Item> _ll{};
    Item *_hand{};

    Sieve(usize cap) : _cap(cap) {}

    ~Sieve() {
        clear();
    }

    void clear() {
        _ll.clearApply([](Item *item) {
            delete item;
        });
    }

    Item *_lookup(K const &key) {
        auto *item = _ll.head();
        while (item) {
            if (item->key == key)
                return item;
            item = _ll.next(item);
        }
        return nullptr;
    }

    void _evict() {
        auto *item = _hand ?: _ll.tail();
        while (item and item->visited) {
            item->visited = false;
            item = _ll.prev(item) ?: _ll.tail();
        }
        _hand = _ll.prev(item) ?: _ll.tail();
        _ll.detach(item);
        delete item;
    }

    V &access(K const &key, auto const &make) {
        auto item = _lookup(key);
        if (item) {
            item->visited = true;
            return item->value;
        }

        if (_ll.len() == _cap)
            _evict();

        item = new Item{key, make()};
        _ll.prepend(item, _ll.head());
        return item->value;
    }

    Opt<V> get(K const &key) {
        auto item = _lookup(key);
        if (item) {
            item->visited = true;
            return item->value;
        }
        return NONE;
    }

    bool has(K const &key) {
        return _lookup(key) != nullptr;
    }

    usize len() const {
        return _ll.len();
    }
};

} // namespace Karm
