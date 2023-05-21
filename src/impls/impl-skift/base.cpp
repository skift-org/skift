#include <hjert-api/api.h>

namespace Embed {

void debug(char const *buf) {
    Hj::log(buf).unwrap();
}

[[noreturn]] void panic(char const *buf) {
    Hj::log(buf).unwrap();
    Hj::Task::self().crash().unwrap();
    __builtin_unreachable();
}

void relaxe() {
#if defined(__x86_64__)
    asm volatile("pause");
#else
#    error "Unsupported architecture"
#endif
}

void enterCritical() {
}

void leaveCritical() {
}

} // namespace Embed
