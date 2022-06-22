#include <karm-base/panic.h>
#include <karm-base/std.h>

void *__attribute__((weak)) operator new(size_t) {
    notImplemented();
}

void *__attribute__((weak)) operator new[](size_t) {
    notImplemented();
}

void __attribute__((weak)) operator delete(void *) {
    notImplemented();
}

void __attribute__((weak)) operator delete[](void *) {
    notImplemented();
}

void __attribute__((weak)) operator delete(void *, size_t) {
    notImplemented();
}

void __attribute__((weak)) operator delete[](void *, size_t) {
    notImplemented();
}
