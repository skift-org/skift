#include <ce-heap/libheap.h>
#include <hjert-core/mem.h>
#include <karm-base/lock.h>
#include <karm-logger/logger.h>

// MARK: Kmm Implementation ---------------------------------------------------

static Lock _heapLock;
static Heap _heapImpl = {
    .ctx = nullptr,
    .alloc = [](void*, usize size) -> void* {
        usize start = Hjert::Core::kmm()
                          .allocRange(size)
                          .unwrap("heap: failed to allocate block")
                          .start;
        return reinterpret_cast<void*>(start);
    },
    .free = [](void*, void* ptr, usize size) {
        Hjert::Core::kmm()
            .free(Hal::KmmRange(reinterpret_cast<usize>(ptr), size))
            .unwrap("heap: failed to free block");
    },
    .log = [](void*, enum HeapLogType type, char const* msg, va_list) {
        if (type == HEAP_ERROR)
            logError("heap: {}", msg);
    },
    .root = nullptr,
    .best = nullptr,
};

// MARK: New/Delete Implementation ---------------------------------------------

void* operator new(usize size) {
    LockScope scope(_heapLock);
    return heap_calloc(&_heapImpl, size, 1);
}

void* operator new[](usize size) {
    LockScope scope(_heapLock);
    return heap_calloc(&_heapImpl, size, 1);
}

void operator delete(void* ptr) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void operator delete[](void* ptr) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void operator delete(void* ptr, usize) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}

void operator delete[](void* ptr, usize) {
    LockScope scope(_heapLock);
    heap_free(&_heapImpl, ptr);
}
