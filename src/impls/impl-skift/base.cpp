#include <hjert-api/syscalls.h>

namespace Embed {

void debug(char const *buf) {
    Hjert::Api::Log::call(buf, strlen(buf)).unwrap();
}

[[noreturn]] void panic(char const *buf) {
    Hjert::Api::Log::call(buf, strlen(buf)).unwrap();
    Hjert::Api::Exit::call(Hjert::Api::SELF, -1).unwrap();
    __builtin_unreachable();
}

void relaxe() {}

void enterCritical() {}

void leaveCritical() {}

} // namespace Embed
