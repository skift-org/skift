#include <hjert-api/api.h>

extern "C" void __entryPoint() {
    (void)Hj::log("Hello from userspace!", 21);

    while (true) {
        asm("pause");
    }
}
