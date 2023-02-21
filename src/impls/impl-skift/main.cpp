#include <karm-main/base.h>

#include <hal-x86_64/com.h>

extern "C" void __entryPoint() {
    x86_64::Com com = x86_64::Com::com1();
    (void)com.writeStr("Hello, world!\n");
    (void)com.writeStr("Hello, world!\n");
    (void)com.writeStr("Hello, world!\n");
    (void)com.writeStr("Hello, world!\n");
    while (true) {
        asm("hlt");
    }
}
