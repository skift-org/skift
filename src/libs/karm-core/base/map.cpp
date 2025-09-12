export module Karm.Core:base.map;

import :base.cursor;
import :base.vec;

namespace Karm {

export template <typename K, typename V>
struct Map {
    Vec<Pair<K, V>> _els{};

    Map() = default;

    Map(std::initializer_list<Pair<K, V>>&& list)
        : _els(std::move(list)) {}

    void put(K const& key, V value) {
        for (auto& i : mutIter(_els)) {
            if (i.v0 == key) {
                i.v1 = std::move(value);
                return;
            }
        }

        _els.pushBack(Pair<K, V>{key, std::move(value)});
    }

    bool has(K const& key) const {
        for (auto& i : _els) {
            if (i.v0 == key) {
                return true;
            }
        }

        return false;
    }

    V& get(K const& key) {
        for (auto& i : _els) {
            if (i.v0 == key) {
                return i.v1;
            }
        }

        panic("key not found");
    }

    V const& get(K const& key) const {
        for (auto& i : _els) {
            if (i.v0 == key) {
                return i.v1;
            }
        }

        panic("key not found");
    }

    V& getOrDefault(K const& key, V const& defaultValue = {}) {
        for (auto& i : _els) {
            if (i.v0 == key) {
                return i.v1;
            }
        }

        _els.pushBack(Pair<K, V>{key, defaultValue});
        return last(_els).v1;
    }

    MutCursor<V> access(K const& key) {
        for (auto& i : _els)
            if (i.v0 == key)
                return &i.v1;
        return {};
    }

    Cursor<V> access(K const& key) const {
        for (auto& i : _els)
            if (i.v0 == key)
                return &i.v1;
        return {};
    }

    V take(K const& key) {
        for (usize i = 0; i < _els.len(); i++) {
            if (_els[i].v0 == key) {
                V value = std::move(_els[i].v1);
                _els.removeAt(i);
                return value;
            }
        }

        panic("key not found");
    }

    Opt<V> tryGet(K const& key) const {
        for (auto& i : _els) {
            if (i.v0 == key) {
                return i.v1;
            }
        }

        return NONE;
    }

    bool del(K const& key) {
        for (usize i = 0; i < _els.len(); i++) {
            if (_els[i].v0 == key) {
                _els.removeAt(i);
                return true;
            }
        }

        return false;
    }

    bool removeAll(V const& value) {
        bool changed = false;

        for (usize i = 1; i < _els.len() + 1; i++) {
            if (_els[i - 1].v1 == value) {
                _els.removeAt(i - 1);
                changed = true;
                i--;
            }
        }

        return changed;
    }

    bool removeFirst(V const& value) {
        for (usize i = 1; i < _els.len() + 1; i++) {
            if (_els[i - 1].v1 == value) {
                _els.removeAt(i - 1);
                return true;
            }
        }

        return false;
    }

    auto iter() {
        return Karm::iter(_els);
    }

    auto iter() const {
        return Karm::iter(_els);
    }

    V at(usize index) const {
        return _els[index].v1;
    }

    usize len() const {
        return _els.len();
    }

    void clear() {
        _els.clear();
    }
};

} // namespace Karm
