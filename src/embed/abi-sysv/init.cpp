#include <karm-base/std.h>

extern "C" void (*__init_array_start[])(int, char **, char **) __attribute__((visibility("hidden")));
extern "C" void (*__init_array_end[])(int, char **, char **) __attribute__((visibility("hidden")));

namespace Abi::SysV {

void init() {
    const size_t size = __init_array_end - __init_array_start;
    for (size_t i = 0; i < size; i++) {
        (*__init_array_start[i])(0, nullptr, nullptr);
    }
}

} // namespace Abi::SysV
