#pragma once

#include <karm-base/box.h>
#include <karm-base/size.h>
#include <karm-base/time.h>

#include "space.h"

namespace Hjert::Core {

struct Ctx {
    static Res<Box<Ctx>> create(uintptr_t ksp);

    virtual ~Ctx() = default;
    virtual void save() = 0;
    virtual void load() = 0;
};

/* --- Stack ----------------------------------------------------------------- */

struct Stack {
    static constexpr size_t DEFAULT_SIZE = kib(16);

    static Res<Stack> create();

    Hal::KmmMem _mem;
    uintptr_t _sp;

    void saveSp(uintptr_t sp) {
        _sp = sp;
    }

    uintptr_t loadSp() {
        return _sp;
    }

    void push(Bytes bytes) {
        _sp -= bytes.len();
        memcpy((void *)_sp, bytes.buf(), bytes.len());
    }

    template <typename T>
    void push(T t) {
        push(Bytes{reinterpret_cast<uint8_t *>(&t), sizeof(t)});
    }
};

/* --- Task ----------------------------------------------------------------- */

enum struct TaskType : uint8_t {
    IDLE,
    USER,
    KERNEL
};

struct Task : public Object<Task> {
    TaskType _type;
    Stack _stack;
    Strong<Space> _space;
    Strong<Domain> _domain;
    Box<Ctx> _ctx;

    Tick _sliceStart = 0;
    Tick _sliceEnd = 0;

    static Res<Strong<Task>> create(TaskType type, Strong<Space> space);

    static Task &self();

    Task(TaskType type, Stack stack, Strong<Space> space, Strong<Domain> domain, Box<Ctx> ctx)
        : _type(type),
          _stack(std::move(stack)),
          _space(space),
          _domain(domain),
          _ctx(std::move(ctx)) {
    }

    auto type() const { return _type; }

    Stack &stack() { return _stack; }

    Space &space() { return *_space; }

    Domain &domain() { return *_domain; }

    void saveCtx(uintptr_t sp) {
        _stack.saveSp(sp);
        _ctx->save();
    }

    uintptr_t loadCtx() {
        _space->activate();
        _ctx->load();
        return _stack.loadSp();
    }
};

} // namespace Hjert::Core
