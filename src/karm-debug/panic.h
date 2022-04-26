#pragma once

#include <stdio.h>
#include <stdlib.h>

namespace Karm::Debug {

[[noreturn]] static void panic(char const *msg) {
    fprintf(stderr, "PANIC: %s\n", msg);
    *(volatile int *)0 = 0;
    abort();
}

} // namespace Karm::Debug
