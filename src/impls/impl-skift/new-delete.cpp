#include <hjert-api/api.h>
#include <karm-base/lock.h>
#include <karm-logger/logger.h>
#include <libheap/libheap.h>

/* --- Heap Implementation -------------------------------------------------- */

static constinit Lock _heapLock;
static constinit Heap _heapImpl = {
    .alloc = [](void *, usize size) -> void * {
        auto space = Hj::Space::self();
        auto mem = Hj::createMem(Hj::ROOT, 0, size).take();
        return (void *)space.map(mem, 0, size, Hj::MapFlags::READ | Hj::MapFlags::WRITE).unwrap();
    },
    .free = [](void *, void *ptr, usize size) -> void {
        auto space = Hj::Space::self();
        space.unmap((usize)ptr, size).unwrap();
    },
    .log = [](void *, enum HeapLogType type, char const *msg, va_list) -> void {
        if (type == HEAP_ERROR)
            logError("heap: {}", msg);
    },
};

/* --- New/Delete Implementation -------------------------------------------- */

void *operator new(usize size) {
    LockScope scope(_heapLock);
    return heap_calloc(&_heapImpl, size, 1);
}

void *operator new[](usize size) {
    LockScope scope(_heapLock);
    return heap_calloc(&_heapImpl, size, 1);
}

void operator delete(void *ptr) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void operator delete[](void *ptr) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void operator delete(void *ptr, usize) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void operator delete[](void *ptr, usize) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}
