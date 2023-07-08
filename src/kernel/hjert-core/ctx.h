#pragma once

#include <hal/kmm.h>
#include <karm-base/box.h>
#include <karm-base/res.h>
#include <karm-base/size.h>

#include "arch.h"

namespace Hjert::Core {

struct Ctx {
    virtual ~Ctx() = default;
    virtual void save(Arch::Frame const &) = 0;
    virtual void load(Arch::Frame &) = 0;
};

/* --- Stack ---------------------------------------------------------------- */

struct Stack {
    static constexpr usize DEFAULT_SIZE = kib(64);

    static Res<Stack> create();

    Hal::KmmMem _mem;
    usize _sp;

    void saveSp(usize sp);
    usize loadSp();
    void push(Bytes bytes);

    template <typename T>
    void push(T t) {
        push(Bytes{reinterpret_cast<u8 *>(&t), sizeof(t)});
    }
};

} // namespace Hjert::Core
