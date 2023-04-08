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

void relaxe() {
#if defined(__x86_64__)
    asm volatile("pause");
#endif
}

void enterCritical() {
    // NOTE: We don't do any thread so we don't need to do anything here.
}

void leaveCritical() {
    // NOTE: We don't do any thread so we don't need to do anything here.
}

} // namespace Embed
