#include <karm-base/panic.h>

extern "C" int __cxa_atexit(void (*)(void *), void *, void *) {
    return 0;
}

extern "C" void __cxa_pure_virtual() {
    Karm::panic("pure virtual function called");
}