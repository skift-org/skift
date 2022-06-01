#include <stdio.h>
#include <stdlib.h>

namespace Embed {

[[noreturn]] void panicHandler(char const *buf) {
    fprintf(stderr, "PANIC: %s\n", buf);
    abort();
}

} // namespace Embed
