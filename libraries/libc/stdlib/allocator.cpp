#include <abi/Syscalls.h>
#include <assert.h>
#include <skift/Plugs.h>
#include <stdlib.h>
#include <string.h>

#define LIBALLOC_MAGIC 0xc001c0de
#define LIBALLOC_DEAD 0xdeaddead

#define USE_CASE1
#define USE_CASE2
#define USE_CASE3
#define USE_CASE4
#define USE_CASE5

#define MIN(__x, __y) ((__x) < (__y) ? (__x) : (__y))
#define MAX(__x, __y) ((__x) > (__y) ? (__x) : (__y))

struct MinorBlock;

struct MajorBlock
{
    // The number of pages in the block.
    size_t pages;

    // The number of pages in the block.
    size_t size;

    // The number of bytes used in the block.
    size_t usage;

    // Linked list information.
    MajorBlock *prev;

    // Linked list information.
    MajorBlock *next;

    // A pointer to the first allocated memory in the block.
    MinorBlock *first;
};

#define MAJOR_BLOCK_HEADER_SIZE (__align_up(sizeof(MajorBlock), 16))

struct MinorBlock
{
    // A magic number to identify correctness.
    size_t magic;

    // The size of the memory allocated. Could be 1 byte or more.
    size_t size;

    // The size of memory requested.
    size_t req_size;

    // Linked list information.
    MinorBlock *prev;

    // Linked list information.
    MinorBlock *next;

    // The owning block. A pointer to the major structure.
    MajorBlock *block;
};

#define MINOR_BLOCK_HEADER_SIZE (__align_up(sizeof(MinorBlock), 16))

// The root memory block acquired from the system.
static MajorBlock *_heap_root = nullptr;

// The major with the most free memory.
static MajorBlock *_best_bet = nullptr;

// The size of an individual page. Set up in liballoc_init.
static constexpr size_t _page_size = 4096;

// The number of pages to request per chunk. Set up in liballoc_init.
static constexpr size_t _page_count = 16;

static MajorBlock *heap_major_block_create(size_t size)
{
    // This is how much space is required.
    size_t st = size + MAJOR_BLOCK_HEADER_SIZE;
    st += MINOR_BLOCK_HEADER_SIZE;

    // Perfect amount of space?
    if ((st % _page_size) == 0)
    {
        st = st / (_page_size);
    }
    else
    {
        st = st / (_page_size) + 1;
    }

    // Make sure it's >= the minimum size.
    st = MAX(st, _page_count);

    MajorBlock *maj = (MajorBlock *)__plug_memory_alloc(st * _page_size);

    if (maj == nullptr)
    {
        return nullptr;
    }

    maj->prev = nullptr;
    maj->next = nullptr;
    maj->pages = st;
    maj->size = st * _page_size;
    maj->usage = MAJOR_BLOCK_HEADER_SIZE;
    maj->first = nullptr;

    return maj;
}

bool check_minor_magic(MinorBlock *min, void *ptr, void *caller)
{
    if (min->magic == LIBALLOC_MAGIC)
    {
        return true;
    }

    // Check for overrun errors. For all bytes of LIBALLOC_MAGIC
    if (((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) ||
        ((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) ||
        ((min->magic & 0xFF) == (LIBALLOC_MAGIC & 0xFF)))
    {
        __unused(caller);
        __unused(ptr);
        // logger_error("Possible 1-3 byte overrun for magic 0x%x != 0x%x from 0x%x.",
        //              min->magic,
        //              LIBALLOC_MAGIC,
        //              caller);
    }

    if (min->magic == LIBALLOC_DEAD)
    {
        // logger_error("Multiple free(0x%x) attempt from 0x%x.",
        //              ptr,
        //              caller);
    }
    else
    {
        // logger_error("Bad free(0x%x) from 0x%x", ptr, caller);
    }

    __plug_memory_unlock();

    return false;
}

void *malloc(size_t req_size)
{
    req_size = __align_up(req_size, 16);

    unsigned long long bestSize = 0;
    unsigned long size = req_size;

    __plug_memory_lock();

    if (size == 0)
    {
        // logger_warn("alloc(0) called from 0x%x", __builtin_return_address(0));
        __plug_memory_unlock();

        return malloc(1);
    }

    // Is this the first time we are being used?
    if (_heap_root == nullptr)
    {
        _heap_root = heap_major_block_create(size);

        if (_heap_root == nullptr)
        {
            __plug_memory_unlock();
            return nullptr;
        }
    }

    // Now we need to bounce through every major and find enough space....
    MajorBlock *maj = _heap_root;
    bool started_with_bestbet = false;

    // Start at the best bet....
    if (_best_bet != nullptr)
    {
        bestSize = _best_bet->size - _best_bet->usage;

        if (bestSize > (size + MINOR_BLOCK_HEADER_SIZE))
        {
            maj = _best_bet;
            started_with_bestbet = true;
        }
    }

    while (maj != nullptr)
    {
        size_t diff = maj->size - maj->usage;
        // free memory in the block

        if (bestSize < diff)
        {
            // Hmm.. this one has more memory then our bestBet. Remember!
            _best_bet = maj;
            bestSize = diff;
        }

#ifdef USE_CASE1

        // CASE 1:  There is not enough space in this major block.
        if (diff < (size + MINOR_BLOCK_HEADER_SIZE))
        {

            // Another major block next to this one?
            if (maj->next != nullptr)
            {
                maj = maj->next; // Hop to that one.
                continue;
            }

            if (started_with_bestbet)
            {
                // let's start all over again.
                maj = _heap_root;
                started_with_bestbet = false;
                continue;
            }

            // Create a new major block next to this one and...
            maj->next = heap_major_block_create(size);

            if (maj->next == nullptr)
            {
                break; // no more memory :sad:
            }

            maj->next->prev = maj;
            maj = maj->next;

            // .. fall through to CASE 2 ..
        }

#endif

#ifdef USE_CASE2

        // CASE 2: It's a brand new block.
        if (maj->first == nullptr)
        {
            maj->first = (MinorBlock *)((uintptr_t)maj + MAJOR_BLOCK_HEADER_SIZE);

            maj->first->magic = LIBALLOC_MAGIC;
            maj->first->prev = nullptr;
            maj->first->next = nullptr;
            maj->first->block = maj;
            maj->first->size = size;
            maj->first->req_size = req_size;
            maj->usage += size + MINOR_BLOCK_HEADER_SIZE;

            void *p = (void *)((uintptr_t)(maj->first) + MINOR_BLOCK_HEADER_SIZE);

            __plug_memory_unlock();
            return p;
        }

#endif

#ifdef USE_CASE3
        {
            // CASE 3: Block in use and enough space at the start of the block.
            size_t diff = (uintptr_t)(maj->first);
            diff -= (uintptr_t)maj;
            diff -= MAJOR_BLOCK_HEADER_SIZE;

            if (diff >= (size + MINOR_BLOCK_HEADER_SIZE))
            {
                // Yes, space in front. Squeeze in.
                maj->first->prev = (MinorBlock *)((uintptr_t)maj + MAJOR_BLOCK_HEADER_SIZE);
                maj->first->prev->next = maj->first;
                maj->first = maj->first->prev;

                maj->first->magic = LIBALLOC_MAGIC;
                maj->first->prev = nullptr;
                maj->first->block = maj;
                maj->first->size = size;
                maj->first->req_size = req_size;
                maj->usage += size + MINOR_BLOCK_HEADER_SIZE;

                void *p = (void *)((uintptr_t)(maj->first) + MINOR_BLOCK_HEADER_SIZE);
                __plug_memory_unlock(); // release the lock
                return p;
            }
        }

#endif

#ifdef USE_CASE4

        // CASE 4: There is enough space in this block. But is it contiguous?
        MinorBlock *min = maj->first;

        // Looping within the block now...
        while (min != nullptr)
        {
            // CASE 4.1: End of minors in a block. Space from last and end?
            if (min->next == nullptr)
            {
                // the rest of this block is free...  is it big enough?
                size_t diff = (uintptr_t)(maj) + maj->size;
                diff -= (uintptr_t)min;
                diff -= MINOR_BLOCK_HEADER_SIZE;
                diff -= min->size;
                // minus already existing usage..

                if (diff >= (size + MINOR_BLOCK_HEADER_SIZE))
                {
                    min->next = (MinorBlock *)((uintptr_t)min + MINOR_BLOCK_HEADER_SIZE + min->size);
                    min->next->prev = min;
                    min = min->next;
                    min->next = nullptr;
                    min->magic = LIBALLOC_MAGIC;
                    min->block = maj;
                    min->size = size;
                    min->req_size = req_size;
                    maj->usage += size + MINOR_BLOCK_HEADER_SIZE;

                    void *p = (void *)((uintptr_t)min + MINOR_BLOCK_HEADER_SIZE);
                    __plug_memory_unlock(); // release the lock
                    return p;
                }
            }

            // CASE 4.2: Is there space between two minors?
            if (min->next != nullptr)
            {
                // is the difference between here and next big enough?
                size_t diff = (uintptr_t)(min->next);
                diff -= (uintptr_t)min;
                diff -= MINOR_BLOCK_HEADER_SIZE;
                diff -= min->size;
                // minus our existing usage.

                if (diff >= (size + MINOR_BLOCK_HEADER_SIZE))
                {
                    MinorBlock *new_min = (MinorBlock *)((uintptr_t)min + MINOR_BLOCK_HEADER_SIZE + min->size);

                    new_min->magic = LIBALLOC_MAGIC;
                    new_min->next = min->next;
                    new_min->prev = min;
                    new_min->size = size;
                    new_min->req_size = req_size;
                    new_min->block = maj;
                    min->next->prev = new_min;
                    min->next = new_min;
                    maj->usage += size + MINOR_BLOCK_HEADER_SIZE;

                    void *p = (void *)((uintptr_t)new_min + MINOR_BLOCK_HEADER_SIZE);

                    __plug_memory_unlock();

                    return p;
                }
            } // min->next != nullptr

            min = min->next;
        } // while min != nullptr ...

#endif

#ifdef USE_CASE5

        // CASE 5: Block full! Ensure next block and loop.
        if (maj->next == nullptr)
        {

            if (started_with_bestbet)
            {
                // let's start all over again.
                maj = _heap_root;
                started_with_bestbet = false;
                continue;
            }

            // we've run out. we need more...
            // next one guaranteed to be okay
            maj->next = heap_major_block_create(size);

            if (maj->next == nullptr)
            {
                //  uh oh,  no more memory.....
                break;
            }

            maj->next->prev = maj;
        }

#endif

        maj = maj->next;
    } // while (maj != nullptr)

    __plug_memory_unlock();

    // logger_warn("All cases exhausted. No memory available.");

    return nullptr;
}

void free(void *ptr)
{
    if (ptr == nullptr)
    {
        // logger_warn("free( nullptr ) called from 0x%x", __builtin_return_address(0));
        return;
    }

    __plug_memory_lock();

    MinorBlock *min = (MinorBlock *)((uintptr_t)ptr - MINOR_BLOCK_HEADER_SIZE);

    if (!check_minor_magic(min, ptr, __builtin_return_address(0)))
    {
        return;
    }

    MajorBlock *maj = min->block;

    maj->usage -= (min->size + MINOR_BLOCK_HEADER_SIZE);
    min->magic = LIBALLOC_DEAD;

    if (min->next != nullptr)
    {
        min->next->prev = min->prev;
    }

    if (min->prev != nullptr)
    {
        min->prev->next = min->next;
    }

    if (min->prev == nullptr)
    {
        maj->first = min->next;
    }

    if (maj->first == nullptr)
    {
        if (_heap_root == maj)
        {
            _heap_root = maj->next;
        }

        if (_best_bet == maj)
        {
            _best_bet = nullptr;
        }

        if (maj->prev != nullptr)
        {
            maj->prev->next = maj->next;
        }

        if (maj->next != nullptr)
        {
            maj->next->prev = maj->prev;
        }

        __plug_memory_free(maj, maj->pages * _page_size);
    }
    else
    {
        if (_best_bet != nullptr)
        {
            int bestSize = _best_bet->size - _best_bet->usage;
            int majSize = maj->size - maj->usage;

            if (majSize > bestSize)
            {
                _best_bet = maj;
            }
        }
    }

    __plug_memory_unlock();
}

void malloc_cleanup(void *buffer)
{
    if (*(void **)buffer)
    {
        free(*(void **)buffer);
        *(void **)buffer = nullptr;
    }
}

__attribute__((optimize("O0"))) void *calloc(size_t nobj, size_t size)
{
    size_t real_size = nobj * size;

    assert(size != 0 && real_size / size == nobj);

    void *p = malloc(real_size);
    memset(p, 0, real_size);

    return p;
}

void *realloc(void *ptr, size_t size)
{
    size = __align_up(size, 16);

    if (size == 0)
    {
        free(ptr);

        return nullptr;
    }

    if (ptr == nullptr)
    {
        return malloc(size);
    }

    __plug_memory_lock();

    MinorBlock *min = (MinorBlock *)((uintptr_t)ptr - MINOR_BLOCK_HEADER_SIZE);

    if (!check_minor_magic(min, ptr, __builtin_return_address(0)))
    {
        __plug_memory_unlock();
        return nullptr;
    }

    if (min->size >= size)
    {
        min->req_size = size;
        __plug_memory_unlock();
        return ptr;
    }

    __plug_memory_unlock();

    void *new_ptr = malloc(size);
    memcpy(new_ptr, ptr, min->req_size);
    free(ptr);

    return new_ptr;
}
