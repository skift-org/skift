#pragma once

namespace Karm::Base
{

template <typename T>
struct Ref
{
    T *_ptr;

    Ref() = delete;
    Ref(T &ref) : _ptr(&ref) {}
    operator T &() { return *_ptr; }
};

} // namespace Karm::Base
