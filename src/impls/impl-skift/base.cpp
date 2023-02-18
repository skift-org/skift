#include <hjert-api/api.h>

namespace Embed {

void debug(char const *buf) {
    Hj::log(buf, strlen(buf)).unwrap();
}

[[noreturn]] void panic(char const *buf) {
    Hj::log(buf, strlen(buf)).unwrap();
    Hj::ret(Hj::SELF, -1).unwrap();
    __builtin_unreachable();
}

void relaxe() {}

void enterCritical() {}

void leaveCritical() {}

} // namespace Embed
