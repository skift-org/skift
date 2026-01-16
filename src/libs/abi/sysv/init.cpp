export module Abi.SysV;

import Karm.Core;

using namespace Karm;

using InitFunc = void (*)();

extern "C" InitFunc __init_array_start[] __attribute__((visibility("hidden")));
extern "C" InitFunc __init_array_end[] __attribute__((visibility("hidden")));

extern "C" InitFunc __fini_array_start[] __attribute__((visibility("hidden")));
extern "C" InitFunc __fini_array_end[] __attribute__((visibility("hidden")));

namespace Abi::SysV {

export void init() {
    usize size = __init_array_end - __init_array_start;
    for (usize i = 0; i < size; i++) {
        (*__init_array_start[i])();
    }
}

export void fini() {
    usize size = __fini_array_end - __fini_array_start;
    for (usize i = 0; i < size; i++) {
        (*__fini_array_start[i])();
    }
}

} // namespace Abi::SysV
