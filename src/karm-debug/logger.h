#include <stdio.h>
#include <stdlib.h>

namespace Karm::Debug {

static inline void log(char const *msg) {
    fprintf(stderr, "PANIC: %s\n", msg);
}

} // namespace Karm::Debug
