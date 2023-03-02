#include <karm-base/lock.h>
#include <karm-logger/logger.h>
#include <libheap/libheap.h>

/* --- Heap Implementation -------------------------------------------------- */

static Lock _heapLock;
static Heap _heapImpl = {
    .alloc = [](void *, usize) -> void * {
        notImplemented();
    },
    .free = [](void *, void *, usize) -> void {
        notImplemented();
    },
    .log = [](void *, enum HeapLogType type, const char *msg, va_list) -> void {
        if (type == HEAP_ERROR) {
            logError("heap: {}", msg);
        }
    },
};

/* --- New/Delete Implementation -------------------------------------------- */

void *__attribute__((weak)) operator new(usize size) {
    LockScope scope(_heapLock);
    return heap_calloc(&_heapImpl, size, 1);
}

void *__attribute__((weak)) operator new[](usize size) {
    LockScope scope(_heapLock);
    return heap_calloc(&_heapImpl, size, 1);
}

void __attribute__((weak)) operator delete(void *ptr) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void __attribute__((weak)) operator delete[](void *ptr) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void __attribute__((weak)) operator delete(void *ptr, usize) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void __attribute__((weak)) operator delete[](void *ptr, usize) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}
