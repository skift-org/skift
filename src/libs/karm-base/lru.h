#pragma once

#include "list.h"
#include "map.h"

namespace Karm {

template <typename K, typename V>
struct Lru {
    struct Item {
        V value;
        LlItem<Item> item{};
    };

    usize _cap;
    Map<K, Item *> _map;
    Ll<Item> _ll;

    Lru(usize cap) : _cap(cap) {}

    ~Lru() {
        clear();
    }

    void clear() {
        _map.clear();
        _ll.clearApply([](Item *item) {
            delete item;
        });
    }

    Item *_lookup(K const &key) {
        Opt<Item *> item = _map.tryGet(key);
        if (item.has()) {
            _ll.detach(*item);
            _ll.prepend(*item, _ll.head());
            return *item;
        }
        return nullptr;
    }

    void _evict() {
        while (_ll.len() > _cap) {
            auto *item = _ll.tail();
            _ll.detach(item);
            _map.removeFirst(item);
            delete item;
        }
    }

    V &access(K const &key, auto const &make) {
        auto item = _lookup(key);
        if (item) {
            return item->value;
        }

        item = new Item{make()};
        _ll.prepend(item, _ll.head());
        _map.put(key, item);
        _evict();
        return item->value;
    }

    Opt<V> tryGet(K const &key) {
        auto item = _lookup(key);
        if (item) {
            return item->value;
        }
        return NONE;
    }

    bool contains(K const &key) {
        return _lookup(key) != nullptr;
    }

    usize len() const {
        return _ll.len();
    }
};

} // namespace Karm
