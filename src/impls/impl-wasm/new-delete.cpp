#include <ce-heap/libheap.h>
#include <karm-base/lock.h>
#include <karm-logger/logger.h>

#include "externs.h"

// MARK: WASM Implementation ---------------------------------------------------


static Lock _heapLock;
static Heap _heapImpl = {
    .ctx = nullptr,
    .alloc = [](void *, usize size) -> void * {
        return embedAlloc(size);
    },
    .free = [](void *, void *, usize) {
        // Yeet
    },
    .log = [](void *, enum HeapLogType type, char const *msg, va_list) {
        if (type == HEAP_ERROR)
            logError("heap: {}", msg);
    },
    .root = nullptr,
    .best = nullptr,
};

// MARK: New/Delete Implementation ---------------------------------------------

void *operator new(usize size) {
    LockScope scope(_heapLock);
    return heap_calloc(&_heapImpl, size, 2);
}

void *operator new[](usize size) {
    LockScope scope(_heapLock);
    return heap_calloc(&_heapImpl, size, 2);
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
