#pragma once

#include <karm-meta/traits.h>

#include "cursor.h"
#include "lock.h"
#include "opt.h"

namespace Karm {

/// A reference-counted object heap cell.
template <typename L>
struct _Cell {
    i32 _strong = 0;
    i32 _weak = 0;
    [[no_unique_address]] L _lock;

    virtual ~_Cell() = default;

    virtual void* _unwrap() lifetimebound = 0;

    virtual void clear() = 0;

    virtual Meta::Id id() = 0;

    void collectAndRelease(bool collect) {
        if (_strong == 0 and collect) {
            clear();
        }
        _lock.release();
        if (_strong == 0 and _weak == 0)
            delete this;
    }

    _Cell* refStrong() lifetimebound {
        LockScope scope(_lock);

        _strong++;
        if (_strong < 0) [[unlikely]]
            panic("refStrong() overflow");

        return this;
    }

    void derefStrong() {
        _lock.acquire();

        _strong--;
        if (_strong < 0) [[unlikely]]
            panic("derefStrong() underflow");

        collectAndRelease(true);
    }

    _Cell* refWeak() lifetimebound {
        LockScope scope(_lock);

        _weak++;
        if (_weak < 0) [[unlikely]]
            panic("refWeak() overflow");

        return this;
    }

    void derefWeak() {
        _lock.acquire();

        _weak--;
        if (_weak < 0) [[unlikely]]
            panic("derefWeak() underflow");

        collectAndRelease(false);
    }

    template <typename T>
    T& unwrap() lifetimebound {
        return *static_cast<T*>(_unwrap());
    }
};

template <typename L, typename T>
struct Cell : public _Cell<L> {
    Manual<T> _buf{};

    template <typename... Args>
    Cell(Args&&... args) {
        _buf.ctor(std::forward<Args>(args)...);
    }

    void* _unwrap() lifetimebound override {
        return &_buf.unwrap();
    }

    Meta::Id id() override {
        return Meta::idOf<T>();
    }

    void clear() override {
        _buf.dtor();
    }
};

/// A strong reference to an object of type  `T`.
///
/// A strong reference keeps the object alive as long as the
/// reference is in scope. When the reference goes out of scope
/// the object is deallocated if there are no other strong
/// references to it.
template <typename L, typename T>
struct _Rc {
    _Cell<L>* _cell{};

    // MARK: Rule of Five ------------------------------------------------------

    constexpr _Rc() = delete;

    constexpr _Rc(Move, _Cell<L>* ptr)
        : _cell(ptr->refStrong()) {
    }

    constexpr _Rc(_Rc const& other)
        : _cell(other._cell->refStrong()) {
    }

    constexpr _Rc(_Rc&& other)
        : _cell(std::exchange(other._cell, nullptr)) {
    }

    template <Meta::Derive<T> U>
    constexpr _Rc(_Rc<L, U> const& other)
        : _cell(other._cell->refStrong()) {
    }

    template <Meta::Derive<T> U>
    constexpr _Rc(_Rc<L, U>&& other)
        : _cell(std::exchange(other._cell, nullptr)) {
    }

    constexpr ~_Rc() {
        if (_cell) {
            _cell->derefStrong();
            _cell = nullptr;
        }
    }

    constexpr _Rc& operator=(_Rc const& other) {
        *this = _Rc(other);
        return *this;
    }

    constexpr _Rc& operator=(_Rc&& other) {
        std::swap(_cell, other._cell);
        return *this;
    }

    // MARK: Operators ---------------------------------------------------------

    constexpr T const* operator->() const lifetimebound {
        return &unwrap();
    }

    constexpr T* operator->() lifetimebound {
        return &unwrap();
    }

    constexpr T const& operator*() const lifetimebound {
        return unwrap();
    }

    constexpr T& operator*() lifetimebound {
        return unwrap();
    }

    // MARK: Methods -----------------------------------------------------------

    /// Returns the number of strong references to the object.
    constexpr usize strong() const {
        return _cell ? _cell->_strong : 0;
    }

    /// Returns the number of weak references to the object.
    constexpr usize weak() const {
        return _cell ? _cell->_weak : 0;
    }

    /// Returns the total number of references to the object.
    constexpr usize refs() const {
        return strong() + weak();
    }

    constexpr void ensure() const {
        if (not _cell) [[unlikely]]
            panic("null dereference");
    }

    constexpr T const& unwrap() const lifetimebound {
        ensure();
        return _cell->template unwrap<T>();
    }

    constexpr T& unwrap() lifetimebound {
        ensure();
        return _cell->template unwrap<T>();
    }

    template <Meta::Derive<T> U>
    constexpr U const& unwrap() const lifetimebound {
        ensure();
        if (not is<U>()) [[unlikely]]
            panic("unwrapping T as U");

        return _cell->template unwrap<U>();
    }

    template <Meta::Derive<T> U>
    constexpr U& unwrap() lifetimebound {
        ensure();
        if (not is<U>()) [[unlikely]]
            panic("unwrapping T as U");

        return _cell->template unwrap<U>();
    }

    template <typename U>
    constexpr MutCursor<U> is() {
        if (not _cell)
            return nullptr;

        if (not Meta::Same<T, U> and
            not Meta::Derive<T, U> and
            not(_cell->id() == Meta::idOf<U>())) {
            return nullptr;
        }

        return &_cell->template unwrap<U>();
    }

    template <typename U>
    constexpr Cursor<U> is() const {
        if (not _cell)
            return nullptr;

        if (not Meta::Same<T, U> and
            not Meta::Derive<T, U> and
            not(_cell->id() == Meta::idOf<U>())) {
            return nullptr;
        }

        return &_cell->template unwrap<U>();
    }

    Meta::Id id() const {
        ensure();
        return _cell->id();
    }

    template <typename U>
    constexpr Opt<_Rc<L, U>> cast() {
        if (not is<U>()) {
            return NONE;
        }

        return _Rc<L, U>(MOVE, _cell);
    }

    auto operator<=>(_Rc const& other) const
        requires Meta::Comparable<T>
    {
        if (_cell == other._cell)
            return _cell <=> other._cell;
        return unwrap() <=> other.unwrap();
    }

    bool operator==(_Rc const& other) const
        requires Meta::Equatable<T>
    {
        if (_cell == other._cell)
            return true;
        return unwrap() == other.unwrap();
    }

    auto operator<=>(T const& other) const {
        return unwrap() <=> other;
    }

    bool operator==(T const& other) const {
        return unwrap() == other;
    }
};

/// A weak reference to a an object of type `T`.
///
/// A weak reference does not keep the object alive, but can be
/// upgraded to a strong reference if the object is still alive.
template <typename L, typename T>
struct _Weak {
    _Cell<L>* _cell;

    constexpr _Weak() = delete;

    template <Meta::Derive<T> U>
    constexpr _Weak(_Rc<L, U> const& other)
        : _cell(other._cell->refWeak()) {}

    template <Meta::Derive<T> U>
    constexpr _Weak(_Weak<L, U> const& other)
        : _cell(other._cell->refWeak()) {}

    template <Meta::Derive<T> U>
    constexpr _Weak(_Weak<L, U>&& other)
        : _cell(std::exchange(other._cell, nullptr)) {
    }

    constexpr _Weak(Move, _Cell<L>* ptr)
        : _cell(ptr->refWeak()) {
    }

    constexpr _Weak& operator=(_Rc<L, T> const& other) {
        *this = _Weak(other);
        return *this;
    }

    constexpr _Weak& operator=(_Weak const& other) {
        *this = _Weak(other);
        return *this;
    }

    constexpr _Weak& operator=(_Weak&& other) {
        std::swap(_cell, other._cell);
        return *this;
    }

    constexpr ~_Weak() {
        if (_cell) {
            _cell->derefWeak();
            _cell = nullptr;
        }
    }

    /// Upgrades the weak reference to a strong reference.
    ///
    /// Returns `NONE` if the object has been deallocated.
    Opt<_Rc<L, T>> upgrade() const {
        if (not _cell or _cell->_strong == 0)
            return NONE;
        return _Rc<L, T>(MOVE, _cell);
    }
};

template <typename T>
using Rc = _Rc<NoLock, T>;

template <typename T>
using Weak = _Weak<NoLock, T>;

/// Allocates an object of type `T` on the heap and returns
/// a strong reference to it.
template <typename T, typename... Args>
constexpr static Rc<T> makeRc(Args&&... args) {
    return {MOVE, new Cell<NoLock, T>(std::forward<Args>(args)...)};
}

template <typename T>
using Arc = _Rc<Lock, T>;

template <typename T>
using Aweak = _Weak<Lock, T>;

template <typename T, typename... Args>
constexpr static Arc<T> makeArc(Args&&... args) {
    return {MOVE, new Cell<Lock, T>(std::forward<Args>(args)...)};
}

} // namespace Karm
