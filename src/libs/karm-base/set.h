#pragma once

#include "clamp.h"
#include "hash.h"
#include "manual.h"

namespace Karm {

template <typename T>
struct Set {
    struct Slot : public Manual<T> {
        enum State : u8 {
            FREE,
            USED,
            DEAD,
        };

        State state = State::FREE;
    };

    Slot *_slots = nullptr;
    usize _cap = 0;
    usize _len = 0;

    Set(usize cap = 0) : _cap(cap) {
        if (cap)
            _slots = new Slot[cap];
    }

    ~Set() {
        clear();
    }

    void ensure(usize desired) {
        if (desired <= _cap)
            return;

        if (not _slots) {
            _slots = new Slot[desired];
            _cap = desired;
            return;
        }

        auto *oldSlots = _slots;
        usize oldCap = _cap;

        _slots = new Slot[desired];
        _cap = desired;
        _len = 0;

        for (usize i = 0; i < _cap; i++)
            _slots[i].state = Slot::FREE;

        for (usize i = 0; i < oldCap; i++) {
            if (oldSlots[i].state != Slot::USED)
                continue;
            _put(oldSlots[i].unwrap());
        }

        delete[] oldSlots;
    }

    usize _usage() const {
        if (not _cap)
            return 100;
        return (_len * 100) / _cap;
    }

    void _put(T const &t) {
        usize i = hash(t) % _cap;
        while (_slots[i].state != Slot::FREE) {
            if (_slots[i].unwrap() == t)
                return;
            i = (i + 1) % _cap;
        }

        _slots[i].ctor(t);
        _slots[i].state = Slot::USED;
        _len++;
    }

    void put(T const &t) {
        if (_usage() > 80)
            ensure(max(_cap * 2, 16uz));

        _put(t);
    }

    Slot *_lookup(T const &t) const {
        if (_len == 0)
            return nullptr;

        usize i = hash(t) % _cap;
        while (_slots[i].state != Slot::FREE) {
            auto &s = _slots[i];
            if (s.state == Slot::USED and
                s.unwrap() == t)
                return &s;
            i = (i + 1) % _cap;
        }
        return nullptr;
    }

    bool has(T const &t) const {
        return _lookup(t);
    }

    void del(T const &t) {
        auto *slot = _lookup(t);
        if (not slot)
            return;

        slot->state = Slot::DEAD;
        slot->dtor();
        _len--;
    }

    void clear() {
        if (not _slots)
            return;
        for (usize i = 0; i < _cap; i++)
            if (_slots[i].state == Slot::USED)
                _slots[i].dtor();
        delete[] _slots;

        _slots = nullptr;
        _cap = 0;
        _len = 0;
    }

    auto iter() const {
        return Iter{[&, i = 0uz] mutable -> T const * {
            if (i == _cap)
                return nullptr;

            while (i < _cap and _slots[i].state != Slot::USED)
                i++;

            if (i == _cap)
                return nullptr;

            auto *res = &_slots[i].unwrap();
            i++;
            return res;
        }};
    }

    usize len() const {
        return _len;
    }
};

} // namespace Karm
