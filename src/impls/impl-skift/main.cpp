#include <hjert-api/api.h>

extern "C" void __entryPoint() {
    (void)Hj::log("Hello, world!", 13);

    while (true) {
        asm("pause");
    }
}
