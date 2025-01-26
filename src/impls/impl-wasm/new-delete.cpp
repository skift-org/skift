#include <ce-heap/libheap.h>
#include <karm-base/bits.h>
#include <karm-base/lock.h>
#include <karm-logger/logger.h>

#include "externs.h"
#include "karm-base/align.h"

// MARK: WASM Implementation ---------------------------------------------------

static Heap* _ensureHeap() {
    static Bits _heapBase = [] {
        Bits bits(MutBytes{__heap_base, (usize)__heap_end - (usize)__heap_base});
        bits.fill(false);
        return bits;
    }();
    static Heap _heapImpl = {
        .ctx = nullptr,
        .alloc = [](void*, usize size) -> void* {
            size = alignUp(size, PAGE_SIZE);
            auto range = _heapBase.alloc(size / PAGE_SIZE, 0, false);
            return (void*)(range.unwrap("out-of-memory").start * PAGE_SIZE);
        },
        .free = [](void*, void* ptr, usize size) {
            size = alignUp(size, PAGE_SIZE);
            _heapBase.set(BitsRange{(usize)ptr / PAGE_SIZE, size / PAGE_SIZE}, false);
        },
        .log = [](void*, enum HeapLogType type, char const* msg, va_list) {
            if (type == HEAP_ERROR)
                logError("heap: {}", msg);
        },
        .root = nullptr,
        .best = nullptr,
    };

    return &_heapImpl;
}

// MARK: New/Delete Implementation ---------------------------------------------

void* operator new(usize size) {
    return heap_calloc(_ensureHeap(), size, 2);
}

void* operator new[](usize size) {
    return heap_calloc(_ensureHeap(), size, 2);
}

void operator delete(void* ptr) {
    heap_free(_ensureHeap(), ptr);
}

void operator delete[](void* ptr) {
    heap_free(_ensureHeap(), ptr);
}

void operator delete(void* ptr, usize) {
    heap_free(_ensureHeap(), ptr);
}

void operator delete[](void* ptr, usize) {
    heap_free(_ensureHeap(), ptr);
}
