#pragma once

#include <karm-meta/traits.h>

#include "cursor.h"
#include "lock.h"
#include "opt.h"

namespace Karm {

/// A reference-counted object heap cell.
struct _Cell {
    static constexpr u64 MAGIC = 0xCAFEBABECAFEBABE;

    u64 _magic = MAGIC;
    Lock _lock;
    bool _clear = false;
    isize _strong = 0;
    isize _weak = 0;

    virtual ~_Cell() = default;

    virtual void *_unwrap() = 0;

    virtual void clear() = 0;

    virtual Meta::Type<> inspect() = 0;

    void collectAndRelease() {
        if (_strong == 0 and not _clear) {
            clear();
            _clear = true;
        }

        if (_strong == 0 and _weak == 0) {
            _lock.release();
            delete this;
        } else {
            _lock.release();
        }
    }

    _Cell *refStrong() {
        LockScope scope(_lock);

        if (_clear) [[unlikely]]
            panic("refStrong() called on cleared cell");

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

        collectAndRelease();
    }

    _Cell *refWeak() {
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

        collectAndRelease();
    }

    template <typename T>
    T &unwrap() {
        if (_clear) [[unlikely]]
            panic("unwrap() called on cleared cell");

        return *static_cast<T *>(_unwrap());
    }
};

template <typename T>
struct Cell : public _Cell {
    Manual<T> _buf{};

    template <typename... Args>
    Cell(Args &&...args) {
        _buf.ctor(std::forward<Args>(args)...);
    }

    void *_unwrap() override {
        return &_buf.unwrap();
    }

    Meta::Type<> inspect() override {
        return Meta::typeOf<T>();
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
template <typename T>
struct Strong {
    _Cell *_cell{};

    // MARK: Rule of Five ------------------------------------------------------

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
        if (_cell) {
            _cell->derefStrong();
            _cell = nullptr;
        }
    }

    constexpr Strong &operator=(Strong const &other) {
        *this = Strong(other);
        return *this;
    }

    constexpr Strong &operator=(Strong &&other) {
        std::swap(_cell, other._cell);
        return *this;
    }

    // MARK: Operators ---------------------------------------------------------

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
        if (not is<U>()) [[unlikely]]
            panic("unwrapping T as U");

        return _cell->unwrap<U>();
    }

    template <Meta::Derive<T> U>
    constexpr U &unwrap() {
        ensure();
        if (not is<U>()) [[unlikely]]
            panic("unwrapping T as U");

        return _cell->unwrap<U>();
    }

    template <typename U>
    constexpr MutCursor<U> is() {
        if (not _cell)
            return nullptr;

        if (not Meta::Same<T, U> and
            not Meta::Derive<T, U> and
            not(_cell->inspect().id() == Meta::idOf<U>())) {
            return nullptr;
        }

        return &_cell->unwrap<U>();
    }

    template <typename U>
    constexpr Cursor<U> is() const {
        if (not _cell)
            return nullptr;

        if (not Meta::Same<T, U> and
            not Meta::Derive<T, U> and
            not _cell->inspect().id() == Meta::idOf<U>()) {
            return nullptr;
        }

        return &_cell->unwrap<U>();
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

    auto operator<=>(Strong const &other) const
        requires Meta::Comparable<T>
    {
        if (_cell == other._cell)
            return _cell <=> other._cell;
        return unwrap() <=> other.unwrap();
    }

    bool operator==(Strong const &other) const
        requires Meta::Equatable<T>
    {
        if (_cell == other._cell)
            return true;
        return unwrap() == other.unwrap();
    }

    auto operator<=>(T const &other) const {
        return unwrap() <=> other;
    }

    bool operator==(T const &other) const {
        return unwrap() == other;
    }
};

/// A weak reference to a an object of type `T`.
///
/// A weak reference does not keep the object alive, but can be
/// upgraded to a strong reference if the object is still alive.
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

    constexpr Weak(Move, _Cell *ptr)
        : _cell(ptr->refWeak()) {
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
        if (_cell) {
            _cell->derefWeak();
            _cell = nullptr;
        }
    }

    /// Upgrades the weak reference to a strong reference.
    ///
    /// Returns `NONE` if the object has been deallocated.
    Opt<Strong<T>> upgrade() const {
        if (not _cell or _cell->_clear)
            return NONE;
        return Strong<T>(MOVE, _cell);
    }
};

/// Allocates an object of type `T` on the heap and returns
/// a strong reference to it.
template <typename T, typename... Args>
constexpr static Strong<T> makeStrong(Args &&...args) {
    return {MOVE, new Cell<T>(std::forward<Args>(args)...)};
}

} // namespace Karm
