#pragma once

#include <karm-meta/traits.h>

#include "opt.h"
#include "ordr.h"
#include "ref.h"

namespace Karm {

struct _Cell {
    bool _clear = false;
    int _strong = 0;
    int _weak = 0;

    virtual ~_Cell() = default;

    virtual void *_unwrap() = 0;
    virtual void clear() = 0;
    virtual Meta::Id id() = 0;

    bool dying() {
        return _strong == 0;
    }

    _Cell *collect() {
        if (_strong == 0 and not _clear) {
            clear();
            _clear = true;
        }

        if (_strong == 0 and _weak == 0) {
            delete this;
            return nullptr;
        }

        return this;
    }

    _Cell *refStrong() {
        if (_clear) {
            panic("refStrong() called on cleared cell");
        }

        _strong++;
        if (_strong == 0) {
            panic("refStrong() overflow");
        }
        return this;
    }

    _Cell *derefStrong() {
        _strong--;
        if (_strong < 0) {
            panic("derefStrong() underflow");
        }
        return collect();
    }

    template <typename T>
    T &unwrapStrong() {
        if (_strong == 0) {
            panic("unwrapStrong() called on weak");
        }
        return *static_cast<T *>(_unwrap());
    }

    _Cell *refWeak() {
        _weak++;
        if (_weak == 0) {
            panic("refWeak() overflow");
        }
        return this;
    }

    _Cell *derefWeak() {
        _weak--;
        if (_weak < 0) {
            panic("derefWeak() underflow");
        }
        return collect();
    }

    template <typename T>
    _Cell *unwrapWeak() {
        if (_weak == 0) {
            panic("unwrapWeak()");
        }

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

    Meta::Id id() override { return Meta::makeId<T>(); }

    void clear() override { _buf.dtor(); }
};

inline _Cell *tryRefStrong(_Cell *cell) {
    return cell ? cell->refStrong()
                : nullptr;
}

inline _Cell *tryDerefStrong(_Cell *cell) {
    return cell ? cell->derefStrong()
                : nullptr;
}

template <typename T>
struct Strong {
    _Cell *_cell{};

    constexpr Strong() = delete;

    constexpr Strong(_Cell *ptr)
        : _cell(tryRefStrong(ptr)) {
    }

    constexpr Strong(Strong const &other)
        : _cell(tryRefStrong(other._cell)) {
    }

    constexpr Strong(Strong &&other)
        : _cell(std::exchange(other._cell, nullptr)) {
    }

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> const &other)
        : _cell(tryRefStrong(other._cell)) {
    }

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> &&other)
        : _cell(std::exchange(other._cell, nullptr)) {
    }

    constexpr ~Strong() {
        _cell = tryDerefStrong(_cell);
    }

    constexpr Strong &operator=(Strong const &other) {
        return *this = Strong(other);
    }

    constexpr Strong &operator=(Strong &&other) {
        std::swap(_cell, other._cell);
        return *this;
    }

    constexpr T *operator->() const {
        if (not _cell) {
            panic("Deferencing moved from Strong<T>");
        }

        return &_cell->unwrapStrong<T>();
    }

    constexpr Ordr cmp(Strong const &other) const {
        if (_cell == other._cell)
            return Ordr::EQUAL;

        return ::cmp(unwrap(), other.unwrap());
    }

    constexpr T &operator*() const {
        if (not _cell) {
            panic("Deferencing moved from Strong<T>");
        }

        return _cell->unwrapStrong<T>();
    }

    constexpr T const &unwrap() const {
        if (not _cell) {
            panic("Deferencing moved from Strong<T>");
        }

        return _cell->unwrapStrong<T>();
    }

    constexpr T &unwrap() {
        if (not _cell) {
            panic("Deferencing moved from Strong<T>");
        }

        return _cell->unwrapStrong<T>();
    }

    template <typename U>
    constexpr U &unwrap() {
        if (not _cell) {
            panic("Deferencing moved from Strong<T>");
        }

        if (not is<U>()) {
            panic("Unwrapping Strong<T> as Strong<U>");
        }

        return _cell->unwrapStrong<U>();
    }

    template <typename U>
    constexpr U const &unwrap() const {
        if (not _cell) {
            panic("Deferencing moved from Strong<T>");
        }

        if (not is<U>()) {
            panic("Unwrapping Strong<T> as Strong<U>");
        }

        return _cell->unwrapStrong<U>();
    }

    template <typename U>
    constexpr bool is() {
        return Meta::Same<T, U> or
               Meta::Derive<T, U> or
               _cell->id() == Meta::makeId<U>();
    }

    Meta::Id id() const {
        if (not _cell) {
            panic("Deferencing moved from Strong<T>");
        }

        return _cell->id();
    }

    template <typename U>
    constexpr Opt<Strong<U>> cast() {
        if (not is<U>()) {
            return NONE;
        }

        return Strong<U>(_cell);
    }
};

template <typename T>
struct OptStrong {
    _Cell *_cell{};

    constexpr OptStrong() = default;

    constexpr OptStrong(None) : _cell(nullptr) {}

    constexpr OptStrong(_Cell *ptr) : _cell(tryRefStrong(ptr)) {}

    constexpr OptStrong(OptStrong const &other) : _cell(tryRefStrong(other._cell)) {}

    constexpr OptStrong(OptStrong &&other) : _cell(std::exchange(other._cell, nullptr)) {}

    template <Meta::Derive<T> U>
    constexpr OptStrong(OptStrong<U> const &other) : _cell(tryRefStrong(other._cell)) {}

    template <Meta::Derive<T> U>
    constexpr OptStrong(OptStrong<U> &&other) : _cell(std::exchange(other._cell, nullptr)) {}

    template <Meta::Derive<T> U>
    constexpr OptStrong(Strong<U> const &other) : _cell(tryRefStrong(other._cell)) {}

    template <Meta::Derive<T> U>
    constexpr OptStrong(Strong<U> &&other) : _cell(std::exchange(other._cell, nullptr)) {}

    constexpr ~OptStrong() {
        _cell = tryDerefStrong(_cell);
    }

    constexpr OptStrong &operator=(OptStrong const &other) {
        return *this = Strong(other);
    }

    constexpr OptStrong &operator=(OptStrong &&other) {
        std::swap(_cell, other._cell);
        return *this;
    }

    constexpr operator bool() const { return _cell; }

    constexpr T *operator->() const {
        if (not _cell) {
            panic("Deferencing moved from Strong<T>");
        }

        return &_cell->unwrapStrong<T>();
    }

    constexpr Ordr cmp(OptStrong const &other) const {
        if (_cell == other._cell)
            return Ordr::EQUAL;

        if (not _cell or not other._cell) {
            return Ordr::LESS;
        }

        return ::cmp(unwrap(), other.unwrap());
    }

    constexpr T &operator*() const {
        if (not _cell) {
            panic("Deferencing none OptStrong<T>");
        }

        return _cell->unwrapStrong<T>();
    }

    constexpr T const &unwrap() const {
        if (not _cell) {
            panic("Deferencing none OptStrong<T>");
        }

        return _cell->unwrapStrong<T>();
    }

    constexpr T &unwrap() {
        if (not _cell) {
            panic("Deferencing none OptStrong<T>");
        }

        return _cell->unwrapStrong<T>();
    }

    template <typename U>
    constexpr U &unwrap() {
        if (not _cell) {
            panic("Deferencing none OptStrong<T>");
        }

        if (not is<U>()) {
            panic("Unwrapping Strong<T> as Strong<U>");
        }

        return _cell->unwrapStrong<U>();
    }

    template <typename U>
    constexpr U const &unwrap() const {
        if (not _cell) {
            panic("Deferencing none OptStrong<T>");
        }

        if (not is<U>()) {
            panic("Unwrapping Strong<T> as Strong<U>");
        }

        return _cell->unwrapStrong<U>();
    }

    template <typename U>
    constexpr bool is() {
        return Meta::Same<T, U> or
               Meta::Derive<T, U> or
               _cell->id() == Meta::makeId<U>();
    }

    Meta::Id id() const {
        if (not _cell) {
            panic("Deferencing none OptStrong<T>");
        }

        return _cell->id();
    }

    template <typename U>
    constexpr Opt<Strong<U>> as() {
        if (not is<U>()) {
            return NONE;
        }

        return Strong<U>(_cell);
    }
};

template <typename T, typename... Args>
constexpr static Strong<T> makeStrong(Args &&...args) {
    return {new Cell<T>(std::forward<Args>(args)...)};
}

template <typename T>
struct Weak {
    _Cell *_cell{};

    constexpr Weak() = delete;

    template <Meta::Derive<T> U>
    constexpr Weak(Strong<U> const &other)
        : _cell(other._cell->refWeak()) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> const &other)
        : _cell(other._cell->refWeak()) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> &&other) {
        std::swap(_cell, other._cell);
    }

    constexpr ~Weak() {
        if (_cell) {
            _cell->derefWeak();
        }
    }

    constexpr Weak &operator=(Weak const &other) {
        return *this = Weak(other);
    }

    constexpr Weak &operator=(Weak &&other) {
        std::swap(_cell, other._cell);
        return *this;
    }

    Opt<Strong<T>> upgrade() const {
        if (not _cell) {
            return NONE;
        }

        return Strong<T>(_cell);
    }

    void visit(auto visitor) {
        if (*this) {
            visitor(**this);
        }
    }
};

template <typename T>
using OptWeak = Opt<Weak<T>>;

} // namespace Karm
