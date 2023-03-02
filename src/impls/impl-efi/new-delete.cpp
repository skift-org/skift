#include <efi/base.h>
#include <efi/spec.h>

void *__attribute__((weak)) operator new(usize size) {
    void *res = nullptr;
    Efi::bs()->allocatePool(Efi::MemoryType::BOOT_SERVICES_DATA, size, &res).unwrap("operator new failled");
    return res;
}

void *__attribute__((weak)) operator new[](usize size) {
    void *res = nullptr;
    Efi::bs()->allocatePool(Efi::MemoryType::BOOT_SERVICES_DATA, size, &res).unwrap("operator new[] failled");
    return res;
}

void __attribute__((weak)) operator delete(void *ptr) {
    Efi::bs()->freePool(ptr).unwrap("operator delete failled");
}

void __attribute__((weak)) operator delete[](void *ptr) {
    Efi::bs()->freePool(ptr).unwrap("operator delete[] failled");
}

void __attribute__((weak)) operator delete(void *ptr, usize) {
    Efi::bs()->freePool(ptr).unwrap("operator delete failled");
}

void __attribute__((weak)) operator delete[](void *ptr, usize) {
    Efi::bs()->freePool(ptr).unwrap("operator delete[] failled");
}
