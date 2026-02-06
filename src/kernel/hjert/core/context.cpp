module;

#include <hal/kmm.h>

export module Hjert.Core:context;
import :mem;

import Karm.Core;

namespace Hjert::Arch {

struct Frame;

} // namespace Hjert::Arch

namespace Hjert::Core {

export struct Context {
    virtual ~Context() = default;

    virtual void save(Arch::Frame const& from) = 0;
    virtual void load(Arch::Frame& to) = 0;
};

export struct Stack {
    static constexpr usize DEFAULT_SIZE = 64_KiB;

    static Res<Stack> create() {
        auto mem = try$(kmm().allocOwned(DEFAULT_SIZE));
        auto base = mem.range().end();
        return Ok(Stack{std::move(mem), base});
    }

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
        std::memcpy((void*)_sp, bytes.buf(), bytes.len());
    }

    template <typename T>
    void push(T t) {
        push(Bytes{reinterpret_cast<u8*>(&t), sizeof(t)});
    }
};

} // namespace Hjert::Core
