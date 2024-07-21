#include <ce-heap/libheap.h>
#include <hjert-api/api.h>
#include <karm-base/lock.h>
#include <karm-logger/logger.h>

// MARK: Heap Implementation ---------------------------------------------------

static constinit Lock _heapLock;
static constinit Heap _heapImpl = {
    .ctx = nullptr,
    .alloc = [](void *, usize size) -> void * {
        auto vmo = Hj::Vmo::create(Hj::ROOT, 0, size).take();
        vmo.label("heap").unwrap();

        auto space = Hj::Space::self();
        return (void *)space.map(vmo, 0, size, Hj::MapFlags::READ | Hj::MapFlags::WRITE).unwrap().start;
    },
    .free = [](void *, void *ptr, usize size) -> void {
        auto space = Hj::Space::self();
        space.unmap({(usize)ptr, size}).unwrap();
    },
    .log = [](void *, enum HeapLogType type, char const *msg, va_list) -> void {
        if (type == HEAP_ERROR)
            panic(msg);
    },
    .root = nullptr,
    .best = nullptr,
};

// MARK: New/Delete Implementation ---------------------------------------------

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
