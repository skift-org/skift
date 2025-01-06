#pragma once

#include <karm-base/base.h>
#include <karm-base/vec.h>

namespace Karm::Jit {

enum struct Op : u8 {
};

struct Ref {
    enum struct _Type : u8 {

    };

    using enum _Type;

    _Type type;
};

struct Inst {
    Op op;
    Ref out;
    Ref lhs;
    Ref rhs;
};

struct Block {
    Vec<Inst> insts;
};

} // namespace Karm::Jit
