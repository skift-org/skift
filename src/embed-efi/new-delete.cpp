#include <efi/base.h>
#include <efi/spec.h>

void *__attribute__((weak)) operator new(size_t size) {
    debug("alloc");
    void *res = nullptr;
    Efi::bs()->allocatePool(Efi::MemoryType::BOOT_SERVICES_DATA, size, &res).unwrap("operator new failled");
    return res;
}

void *__attribute__((weak)) operator new[](size_t size) {
    debug("alloc");
    void *res = nullptr;
    Efi::bs()->allocatePool(Efi::MemoryType::BOOT_SERVICES_DATA, size, &res).unwrap("operator new[] failled");
    return res;
}

void __attribute__((weak)) operator delete(void *ptr) {
    debug("free");
    Efi::bs()->freePool(ptr).unwrap("operator delete failled");
}

void __attribute__((weak)) operator delete[](void *ptr) {
    debug("free");
    Efi::bs()->freePool(ptr).unwrap("operator delete[] failled");
}

void __attribute__((weak)) operator delete(void *ptr, size_t) {
    debug("free");
    Efi::bs()->freePool(ptr).unwrap("operator delete failled");
}

void __attribute__((weak)) operator delete[](void *ptr, size_t) {
    debug("free");
    Efi::bs()->freePool(ptr).unwrap("operator delete[] failled");
}
