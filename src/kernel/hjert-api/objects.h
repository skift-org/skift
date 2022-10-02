#pragma once

#include "syscalls.h"
#include "types.h"

namespace Hjert::Api {

struct _Object :
    public Meta::NoCopy {

    RawHandle _rawHandle;

    _Object(RawHandle rawHandle)
        : _rawHandle(rawHandle) {}

    void ref();

    void deref();
};

template <typename Crtp>
struct Object {
    using Handle = Handle<Crtp>;

    Handle _handle;

    Object(Handle handle)
        : _handle(handle) {}

    static Result<Crtp> create(Props<Crtp> props) {
        return try$(Create::call(props)).handle;
    }

    template <typename... Args>
    static Result<Crtp> create(Args... props) {
        return try$(Create::call(Props<Crtp>{std::forward<Args>(props)...})).handle;
    }

    operator Handle() {
        return _handle;
    }
};

struct Task : public Object<Task> {
    constexpr static Type TYPE = Type::TASK;

    Error start(size_t ip, size_t sp, Array<Arg, 5> arg);

    Result<int> wait();

    Error exit(int code);

    Error pledge(Rights rights);

    template <typename T>
    Error grant(AnyHandle what, Rights rights);
};

struct Mem : public Object<Mem> {
    constexpr static Type TYPE = Type::MEM;
};

struct Space : public Object<Space> {
    constexpr static Type TYPE = Type::SPACE;

    Result<USizeRange> map(Mem &mem);

    Error unmap(USizeRange range);
};

} // namespace Hjert::Api
