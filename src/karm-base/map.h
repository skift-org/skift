#pragma once

#include "_prelude.h"

#include "cons.h"
#include "ordr.h"
#include "vec.h"

namespace Karm {

template <typename K, typename V>
struct Map {
    Vec<Cons<K, V>> _els{};

    Map() = default;

    Map(std::initializer_list<Cons<K, V>> &&list)
        : _els(std::move(list)) {}

    void put(K const &key, V const &value) {
        for (auto &i : _els.iter()) {
            if (Op::eq(i.car, key)) {
                i.cdr = value;
                return;
            }
        }

        _els.pushBack(Cons<K, V>{key, value});
    }

    Opt<V> get(K const &key) {
        for (auto &i : _els) {
            if (Op::eq(i.car, key)) {
                return i.cdr;
            }
        }

        return NONE;
    }

    auto iter() {
        return _els.iter();
    }

    auto iter() const {
        return _els.iter();
    }

    size_t len() {
        return _els.len();
    }

    void clear() {
        _els.clear();
    }
};

} // namespace Karm
