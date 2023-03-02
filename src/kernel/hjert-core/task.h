#pragma once

#include <karm-base/box.h>
#include <karm-base/size.h>
#include <karm-base/time.h>

#include "space.h"

namespace Hjert::Core {

struct Ctx {
    static Res<Box<Ctx>> create(usize ksp);

    virtual ~Ctx() = default;
    virtual void save() = 0;
    virtual void load() = 0;
};

/* --- Stack ----------------------------------------------------------------- */

struct Stack {
    static constexpr usize DEFAULT_SIZE = kib(16);

    static Res<Stack> create();

    Hal::KmmMem _mem;
    usize _sp;

    void saveSp(usize sp) {
        _sp = sp;
    }

    usize loadSp() {
        return _sp;
    }

    void push(Bytes bytes) {
        _sp -= bytes.len();
        memcpy((void *)_sp, bytes.buf(), bytes.len());
    }

    template <typename T>
    void push(T t) {
        push(Bytes{reinterpret_cast<u8 *>(&t), sizeof(t)});
    }
};

/* --- Task ----------------------------------------------------------------- */

enum struct TaskType : u8 {
    IDLE,
    USER,
    KERNEL
};

struct Task : public Object<Task> {
    TaskType _type;
    Stack _stack;
    Box<Ctx> _ctx;

    OptStrong<Space> _space;
    OptStrong<Domain> _domain;

    Tick _sliceStart = 0;
    Tick _sliceEnd = 0;

    static Res<Strong<Task>> create(TaskType type, OptStrong<Space> space = NONE, OptStrong<Domain> domain = NONE);

    static Task &self();

    Task(TaskType type, Stack stack, Box<Ctx> ctx, OptStrong<Space> space = NONE, OptStrong<Domain> domain = NONE)
        : _type(type),
          _stack(std::move(stack)),
          _ctx(std::move(ctx)),
          _space(space),
          _domain(domain) {
    }

    auto type() const { return _type; }

    Stack &stack() { return _stack; }

    void saveCtx(usize sp) {
        _stack.saveSp(sp);
        _ctx->save();
    }

    usize loadCtx() {
        if (_space)
            (_space.unwrap()).activate();

        _ctx->load();
        return _stack.loadSp();
    }

    Space &space() { return _space.unwrap("task has no space"); }

    Domain &domain() { return _domain.unwrap("task has no domain"); }
};

} // namespace Hjert::Core
