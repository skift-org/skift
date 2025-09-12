export module Karm.Core:base.set;

import :meta.cvrp;
import :base.clamp;
import :base.hash;
import :base.iter;
import :base.manual;

namespace Karm {

export template <typename T>
struct Set {
    enum State : u8 {
        FREE,
        USED,
        DEAD,
    };

    struct Slot {
        State state = State::FREE;
        Manual<T> value;

        bool clear() {
            if (state != State::USED)
                return false;
            state = State::DEAD;
            value.dtor();
            return true;
        }

        T& unwrap() {
            if (state != State::USED)
                panic("slot is free");
            return value.unwrap();
        }

        T const& unwrap() const {
            if (state != State::USED)
                panic("slot is free");
            return value.unwrap();
        }

        T take() {
            if (state != State::USED)
                panic("slot is free");
            state = State::DEAD;
            return value.take();
        }

        template <typename... Args>
        bool put(Args&&... args) {
            if (state == State::USED) {
                unwrap() = T(std::forward<Args>(args)...);
                return false;
            }

            value.ctor(std::forward<Args>(args)...);
            state = State::USED;
            return true;
        }
    };

    Slot* _slots = nullptr;
    usize _cap = 0;
    usize _len = 0;

    Set(usize cap = 0) : _cap(cap) {
        if (cap)
            _slots = new Slot[cap];
    }

    Set(Set const& other) {
        _cap = other._cap;
        _len = other._len;
        _slots = new Slot[_cap];
        for (usize i = 0; i < _cap; i++) {
            if (other._slots[i].state == State::USED) {
                _slots[i].put(other._slots[i].unwrap());
            } else {
                _slots[i].state = other._slots[i].state;
            }
        }
    }

    Set(Set&& other)
        : _slots(std::exchange(other._slots, nullptr)),
          _cap(std::exchange(other._cap, 0)),
          _len(std::exchange(other._len, 0)) {
    }

    ~Set() {
        if (_slots) {
            clear();
            delete[] _slots;
            _slots = nullptr;
            _cap = 0;
            _len = 0;
        }
    }

    Set& operator=(Set const& other) {
        *this = Set(other);
        return *this;
    }

    Set& operator=(Set&& other) {
        std::swap(_slots, other._slots);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
        return *this;
    }

    usize usage() const {
        if (not _cap)
            return 100;
        return (_len * 100) / _cap;
    }

    void rehash(usize desired) {
        if (desired <= _cap)
            return;

        if (not _slots) {
            _slots = new Slot[desired];
            for (usize i = 0; i < desired; i++)
                _slots[i].state = State::FREE;
            _cap = desired;
            return;
        }

        auto* oldSlots = std::exchange(_slots, new Slot[desired]);
        usize oldCap = std::exchange(_cap, desired);

        for (usize i = 0; i < _cap; i++)
            _slots[i].state = State::FREE;

        for (usize i = 0; i < oldCap; i++) {
            auto& old = oldSlots[i];
            if (old.state != State::USED)
                continue;
            lookup(old.unwrap())->put(old.take());
        }

        delete[] oldSlots;
    }

    void ensureForInsert() {
        if (usage() >= 60)
            rehash(max(_len, _cap * 2, 16uz));
    }

    template <typename Self, Meta::Equatable<T> U>
    auto lookup(this Self& self, U const& u) -> Meta::CopyConst<Self, Slot>* {
        if (not self._slots)
            return nullptr;

        usize start = hash(u) % self._cap;
        usize i = start;
        Meta::CopyConst<Self, Slot>* deadSlot = nullptr;
        while (self._slots[i].state != State::FREE) {
            auto& s = self._slots[i];

            if (s.state == State::USED and
                s.unwrap() == u)
                return &s;

            if (s.state == State::DEAD and not deadSlot)
                deadSlot = &s;

            i = (i + 1) % self._cap;
            if (i == start)
                return nullptr;
        }

        if (deadSlot)
            return deadSlot;

        return &self._slots[i];
    }

    void put(T const& t) {
        auto* slot = lookup(t);
        if (not slot) {
            ensureForInsert();
            slot = lookup(t);
        }
        if (slot->put(t)) {
            _len++;
        }
    }

    bool has(T const& t) const {
        if (auto it = lookup(t); it and it->state == State::USED)
            return true;
        return false;
    }

    void del(T const& t) {
        if (auto it = lookup(t); it and it->state == State::USED) {
            if (it->clear())
                _len--;
        }
    }

    void clear() {
        for (usize i = 0; i < _cap; i++) {
            if (_slots[i].state == State::USED)
                _slots[i].clear();
        }
        _len = 0;
    }

    auto iter() const {
        return Iter{[&, i = 0uz] mutable -> T const* {
            if (i == _cap)
                return nullptr;

            while (i < _cap and _slots[i].state != State::USED)
                i++;

            if (i == _cap)
                return nullptr;

            auto* res = &_slots[i].unwrap();
            i++;
            return res;
        }};
    }

    usize len() const {
        return _len;
    }
};

} // namespace Karm
