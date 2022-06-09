#include <stdio.h>
#include <stdlib.h>

namespace Embed {

void debugHandler(char const *buf) {
    fprintf(stderr, "DEBUG: %s\n", buf);
}

[[noreturn]] void panicHandler(char const *buf) {
    fprintf(stderr, "PANIC: %s\n", buf);
    abort();
}

} // namespace Embed
