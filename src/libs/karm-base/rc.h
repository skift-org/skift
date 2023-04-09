#pragma once

#include <karm-meta/traits.h>

#include "lock.h"
#include "opt.h"
#include "ordr.h"
#include "ref.h"

namespace Karm {

struct _Cell {
    Lock _lock;
    bool _clear = false;
    isize _strong = 0;
    isize _weak = 0;

    virtual ~_Cell() = default;

    virtual void *_unwrap() = 0;
    virtual void clear() = 0;
    virtual Meta::Type<> inspect() = 0;

    _Cell *collect() {
        if (_strong == 0 and not _clear) {
            clear();
            _clear = true;
        }

        if (_strong == 0 and _weak == 0) {
            _lock.release();
            delete this;
            return nullptr;
        } else {
            _lock.release();
            return nullptr;
        }
    }

    _Cell *refStrong() {
        LockScope scope{_lock};

        if (_clear)
            panic("refStrong() called on cleared cell");
        _strong++;
        if (_strong < 0)
            panic("refStrong() overflow");
        return this;
    }

    _Cell *derefStrong() {
        _lock.acquire();

        _strong--;
        if (_strong < 0)
            panic("derefStrong() underflow");
        return collect();
    }

    _Cell *refWeak() {
        LockScope scope{_lock};

        _weak++;
        if (_weak < 0)
            panic("refWeak() overflow");
        return this;
    }

    _Cell *derefWeak() {
        _lock.acquire();

        _weak--;
        if (_weak < 0)
            panic("derefWeak() underflow");
        return collect();
    }

    template <typename T>
    T &unwrap() {
        if (_clear)
            panic("unwrap() called on cleared cell");
        return *static_cast<T *>(_unwrap());
    }
};

template <typename T>
struct Cell : public _Cell {
    Inert<T> _buf{};

    template <typename... Args>
    Cell(Args &&...args) {
        _buf.ctor(std::forward<Args>(args)...);
    }

    void *_unwrap() override { return &_buf.unwrap(); }
    Meta::Type<> inspect() override { return Meta::typeOf<T>(); }
    void clear() override { _buf.dtor(); }
};

template <typename T>
struct Strong {
    _Cell *_cell{};

    /* --- Rule of Five ----------------------------------------------------- */

    constexpr Strong() = delete;

    constexpr Strong(Move, _Cell *ptr)
        : _cell(ptr->refStrong()) {
    }

    constexpr Strong(Strong const &other)
        : _cell(other._cell->refStrong()) {
    }

    constexpr Strong(Strong &&other)
        : _cell(std::exchange(other._cell, nullptr)) {
    }

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> const &other)
        : _cell(other._cell->refStrong()) {
    }

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> &&other)
        : _cell(std::exchange(other._cell, nullptr)) {
    }

    constexpr ~Strong() {
        if (_cell)
            _cell = _cell->derefStrong();
    }

    constexpr Strong &operator=(Strong const &other) {
        *this = Strong(other);
        return *this;
    }

    constexpr Strong &operator=(Strong &&other) {
        std::swap(_cell, other._cell);
        return *this;
    }

    /* --- Operators -------------------------------------------------------- */

    constexpr T const *operator->() const {
        return &unwrap();
    }

    constexpr T *operator->() {
        return &unwrap();
    }

    constexpr T const &operator*() const {
        return unwrap();
    }

    constexpr T &operator*() {
        return unwrap();
    }

    /* --- Methods ---------------------------------------------------------- */

    constexpr void ensure() const {
        if (not _cell) {
            panic("null dereference");
        }
    }

    constexpr T const &unwrap() const {
        ensure();
        return _cell->unwrap<T>();
    }

    constexpr T &unwrap() {
        ensure();
        return _cell->unwrap<T>();
    }

    template <Meta::Derive<T> U>
    constexpr U const &unwrap() const {
        ensure();
        if (not is<U>()) {
            panic("Unwrapping T as U");
        }
        return _cell->unwrap<U>();
    }

    template <Meta::Derive<T> U>
    constexpr U &unwrap() {
        ensure();
        if (not is<U>()) {
            panic("Unwrapping T as U");
        }
        return _cell->unwrap<U>();
    }

    constexpr Ordr cmp(Strong const &other) const {
        if (_cell == other._cell)
            return Ordr::EQUAL;

        return ::cmp(unwrap(), other.unwrap());
    }

    template <typename U>
    constexpr bool is() {
        if (not _cell)
            return false;

        return Meta::Same<T, U> or
               Meta::Derive<T, U> or
               _cell->inspect().id() == Meta::idOf<U>();
    }

    Meta::Type<> inspect() const {
        ensure();
        return _cell->inspect();
    }

    template <typename U>
    constexpr Opt<Strong<U>> cast() {
        if (not is<U>()) {
            return NONE;
        }

        return Strong<U>(MOVE, _cell);
    }
};

template <typename T>
struct Weak {
    _Cell *_cell;

    constexpr Weak() = delete;

    template <Meta::Derive<T> U>
    constexpr Weak(Strong<U> const &other)
        : _cell(other._cell->refWeak()) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> const &other)
        : _cell(other._cell->refWeak()) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> &&other)
        : _cell(std::exchange(other._cell, nullptr)) {
    }

    constexpr Weak &operator=(Strong<T> const &other) {
        *this = Weak(other);
        return *this;
    }

    constexpr Weak &operator=(Weak const &other) {
        *this = Weak(other);
        return *this;
    }

    constexpr Weak &operator=(Weak &&other) {
        std::swap(_cell, other._cell);
        return *this;
    }

    constexpr ~Weak() {
        if (_cell)
            _cell = _cell->derefWeak();
    }

    Opt<Strong<T>> upgrade() const {
        if (not _cell)
            return NONE;
        return Strong<T>(_cell);
    }
};

template <typename T, typename... Args>
constexpr static Strong<T> makeStrong(Args &&...args) {
    return {MOVE, new Cell<T>(std::forward<Args>(args)...)};
}

} // namespace Karm
