export module Karm.Gc:ref;

import Karm.Core;

namespace Karm::Gc {

export template <typename T>
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

template <typename T>
Ref(T) -> Ref<T>;

} // namespace Karm::Gc
