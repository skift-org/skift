#pragma once

#include <hal/kmm.h>
#include <karm-base/box.h>
#include <karm-base/res.h>
#include <karm-base/size.h>

namespace Hjert::Core {

struct Ctx {
    static Res<Box<Ctx>> create(usize ksp);

    virtual ~Ctx() = default;
    virtual void save() = 0;
    virtual void load() = 0;
};

/* --- Stack ---------------------------------------------------------------- */

struct Stack {
    static constexpr usize DEFAULT_SIZE = kib(16);

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
