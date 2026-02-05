#pragma once

import Karm.Core;

#include <hal/kmm.h>

namespace Hjert::Arch {

struct Frame;

} // namespace Hjert::Arch

namespace Hjert::Core {

enum struct Mode : u8 {
    EMBRYO, // The task is being created
    IDLE,   // The task is only run when there is no other task to run
    USER,   // The task is running in user mode
    SUPER,  // The task is running in supervisor mode propably serving a syscall
};

struct Context {
    virtual ~Context() = default;

    virtual void save(Arch::Frame const& from) = 0;
    virtual void load(Arch::Frame& to) = 0;
};

struct Stack {
    static constexpr usize DEFAULT_SIZE = 64_KiB;

    static Res<Stack> create();

    Hal::KmmMem _mem;
    usize _sp;

    void saveSp(usize sp);
    usize loadSp();
    void push(Bytes bytes);

    template <typename T>
    void push(T t) {
        push(Bytes{reinterpret_cast<u8*>(&t), sizeof(t)});
    }
};

} // namespace Hjert::Core
