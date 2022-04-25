#pragma once

namespace Karm::Base
{

template <typename T>
struct Ref
{
    T *_ptr;

    operator T &() { return *_ptr; }
};

} // namespace Karm::Base
