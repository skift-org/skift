#pragma once

#include <karm-base/base.h>
#include <karm-io/emit.h>

namespace Karm::Gc {

template <typename T>
struct Ref {
    T* _ptr = nullptr;

    Ref() = default;

    template <Meta::Derive<T> U>
    Ref(U& ref) : _ptr{&ref} {
    }

    template <Meta::Derive<T> U>
    Ref(Move, U* ptr) : _ptr{ptr} {
        if (not _ptr)
            panic("null pointer");
    }

    template <Meta::Derive<T> U>
    Ref(Ref<U> ref) : Ref{MOVE, ref._ptr} {
    }

    T const* operator->() const {
        return _ptr;
    }

    T* operator->() {
        return _ptr;
    }

    T const& operator*() const {
        return *_ptr;
    }

    T& operator*() {
        return *_ptr;
    }

    void repr(Io::Emit& e) const {
        e("{}", *_ptr);
    }

    bool checkIdentity(Ref const& other) const {
        return _ptr == other._ptr;
    }

    bool operator==(Ref const& other) const {
        return _ptr == other._ptr;
    }
};

// Nullable reference
template <typename T>
struct Ptr {
    T* _ptr = nullptr;

    Ptr() = default;

    Ptr(std::nullptr_t) : _ptr{nullptr} {
    }

    template <Meta::Derive<T> U>
    Ptr(U& ref) : _ptr{&ref} {
    }

    template <Meta::Derive<T> U>
    Ptr(Move, U* ptr) : _ptr{static_cast<T*>(ptr)} {
    }

    template <Meta::Derive<T> U>
    Ptr(Ref<U> ref) : Ptr{MOVE, ref._ptr} {
    }

    template <Meta::Derive<T> U>
    Ptr(Ptr<U> other) : _ptr{other._ptr} {
    }

    T const* operator->() const {
        if (not _ptr)
            panic("trying to dereference a null pointer");
        return _ptr;
    }

    T* operator->() {
        if (not _ptr)
            panic("trying to dereference a null pointer");
        return _ptr;
    }

    T const& operator*() const {
        if (not _ptr)
            panic("trying to dereference a null pointer");
        return *_ptr;
    }

    T& operator*() {
        if (not _ptr)
            panic("trying to dereference a null pointer");
        return *_ptr;
    }

    bool operator==(None) const {
        return not _ptr;
    }

    template <Meta::Comparable<T> U>
    bool operator==(Ptr<U> other) const {
        if (_ptr == other._ptr)
            return true;

        if (not _ptr)
            return false;

        if (not other._ptr)
            return false;

        return *_ptr == *other._ptr;
    }

    template <Meta::Comparable<T> U>
    bool operator==(Ref<U> other) const {
        if (not _ptr)
            return false;

        if (_ptr == other._ptr)
            return true;

        return *_ptr == *other._ptr;
    }

    void repr(Io::Emit& e) const {
        if (not _ptr)
            e("nullptr");
        else
            e("{}", *_ptr);
    }

    explicit operator bool() const {
        return _ptr != nullptr;
    }

    bool checkIdentity(Ptr const& other) const {
        return _ptr == other._ptr;
    }

    Gc::Ref<T> upgrade() const {
        if (not _ptr)
            panic("trying to upgrade a null pointer");
        return {MOVE, _ptr};
    }

    bool operator==(std::nullptr_t) const {
        return not _ptr;
    }

    bool operator==(Ptr const& other) const {
        return _ptr == other._ptr;
    }
};

} // namespace Karm::Gc
