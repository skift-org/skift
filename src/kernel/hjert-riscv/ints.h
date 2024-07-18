#pragma once

#include <karm-base/base.h>

namespace Hjert::Arch {

struct Frame {
    usize ra, gp, tp, t0, t1, t2, t3, t4, t5, t6, a0, a1, a2, a3, a4, a5, a6, a7,
        s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, sp;
};

} // namespace Hjert::Arch
