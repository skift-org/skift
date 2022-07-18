#include <stdio.h>
#include <stdlib.h>

namespace Embed {

void debug(char const *buf) {
    fprintf(stderr, "DEBUG: %s\n", buf);
}

[[noreturn]] void panic(char const *buf) {
    fprintf(stderr, "PANIC: %s\n", buf);
    abort();
}

} // namespace Embed
