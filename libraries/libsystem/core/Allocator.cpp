
/* __alloc__.c : based on durand's Amazing Super Duper Memory functions.      */

#include <libsystem/core/Plugs.h>

#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>

#define LIBALLOC_MAGIC 0xc001c0de
#define LIBALLOC_DEAD 0xdeaddead

#define USE_CASE1
#define USE_CASE2
#define USE_CASE3
#define USE_CASE4
#define USE_CASE5

struct MinorBlock;

struct MajorBlock
{
    unsigned int pages; ///< The number of pages in the block.
    unsigned int size;  ///< The number of pages in the block.
    unsigned int usage; ///< The number of bytes used in the block.

    MajorBlock *prev; ///< Linked list information.
    MajorBlock *next; ///< Linked list information.

    MinorBlock *first; ///< A pointer to the first allocated memory in the block.
};

struct MinorBlock
{
    unsigned int magic; ///< A magic number to identify correctness.

    unsigned int size;     ///< The size of the memory allocated. Could be 1 byte or more.
    unsigned int req_size; ///< The size of memory requested.

    MinorBlock *prev; ///< Linked list information.
    MinorBlock *next; ///< Linked list information.

    MajorBlock *block; ///< The owning block. A pointer to the major structure.
};

static MajorBlock *l_memRoot = nullptr; ///< The root memory block acquired from the system.
static MajorBlock *l_bestBet = nullptr; ///< The major with the most free memory.

static unsigned int l_pageSize = 4096; ///< The size of an individual page. Set up in liballoc_init.
static unsigned int l_pageCount = 16;  ///< The number of pages to request per chunk. Set up in liballoc_init.

static MajorBlock *allocate_new_page(unsigned int size)
{
    // This is how much space is required.
    unsigned int st = size + sizeof(MajorBlock);
    st += sizeof(MinorBlock);

    // Perfect amount of space?
    if ((st % l_pageSize) == 0)
        st = st / (l_pageSize);
    else
        st = st / (l_pageSize) + 1;

    // Make sure it's >= the minimum size.
    if (st < l_pageCount)
        st = l_pageCount;

    MajorBlock *maj = (MajorBlock *)__plug_memalloc_alloc(st * l_pageSize);

    if (maj == nullptr)
    {
        logger_warn("__plug_memalloc_alloc( %i ) return nullptr", st);
        return nullptr;
    }

    maj->prev = nullptr;
    maj->next = nullptr;
    maj->pages = st;
    maj->size = st * l_pageSize;
    maj->usage = sizeof(MajorBlock);
    maj->first = nullptr;

    return maj;
}

void *malloc(size_t req_size)
{
    unsigned long long bestSize = 0;
    unsigned long size = req_size;

    __plug_memalloc_lock();

    if (size == 0)
    {
        logger_warn("alloc( 0 ) called from 0x%x", __builtin_return_address(0));
        __plug_memalloc_unlock();

        return malloc(1);
    }

    // Is this the first time we are being used?
    if (l_memRoot == nullptr)
    {
        l_memRoot = allocate_new_page(size);

        if (l_memRoot == nullptr)
        {
            __plug_memalloc_unlock();
            return nullptr;
        }
    }

    // Now we need to bounce through every major and find enough space....

    MajorBlock *maj = l_memRoot;
    int startedBet = 0;

    // Start at the best bet....
    if (l_bestBet != nullptr)
    {
        bestSize = l_bestBet->size - l_bestBet->usage;

        if (bestSize > (size + sizeof(MinorBlock)))
        {
            maj = l_bestBet;
            startedBet = 1;
        }
    }

    while (maj != nullptr)
    {
        uintptr_t diff = maj->size - maj->usage;
        // free memory in the block

        if (bestSize < diff)
        {
            // Hmm.. this one has more memory then our bestBet. Remember!
            l_bestBet = maj;
            bestSize = diff;
        }

#ifdef USE_CASE1

        // CASE 1:  There is not enough space in this major block.
        if (diff < (size + sizeof(MinorBlock)))
        {

            // Another major block next to this one?
            if (maj->next != nullptr)
            {
                maj = maj->next; // Hop to that one.
                continue;
            }

            if (startedBet == 1) // If we started at the best bet,
            {                    // let's start all over again.
                maj = l_memRoot;
                startedBet = 0;
                continue;
            }

            // Create a new major block next to this one and...
            maj->next = allocate_new_page(size); // next one will be okay.
            if (maj->next == nullptr)
                break; // no more memory.
            maj->next->prev = maj;
            maj = maj->next;

            // .. fall through to CASE 2 ..
        }

#endif

#ifdef USE_CASE2

        // CASE 2: It's a brand new block.
        if (maj->first == nullptr)
        {
            maj->first = (MinorBlock *)((uintptr_t)maj + sizeof(MajorBlock));

            maj->first->magic = LIBALLOC_MAGIC;
            maj->first->prev = nullptr;
            maj->first->next = nullptr;
            maj->first->block = maj;
            maj->first->size = size;
            maj->first->req_size = req_size;
            maj->usage += size + sizeof(MinorBlock);

            void *p = (void *)((uintptr_t)(maj->first) + sizeof(MinorBlock));

            __plug_memalloc_unlock(); // release the lock
            return p;
        }

#endif

#ifdef USE_CASE3

        // CASE 3: Block in use and enough space at the start of the block.
        diff = (uintptr_t)(maj->first);
        diff -= (uintptr_t)maj;
        diff -= sizeof(MajorBlock);

        if (diff >= (size + sizeof(MinorBlock)))
        {
            // Yes, space in front. Squeeze in.
            maj->first->prev = (MinorBlock *)((uintptr_t)maj + sizeof(MajorBlock));
            maj->first->prev->next = maj->first;
            maj->first = maj->first->prev;

            maj->first->magic = LIBALLOC_MAGIC;
            maj->first->prev = nullptr;
            maj->first->block = maj;
            maj->first->size = size;
            maj->first->req_size = req_size;
            maj->usage += size + sizeof(MinorBlock);

            void *p = (void *)((uintptr_t)(maj->first) + sizeof(MinorBlock));
            __plug_memalloc_unlock(); // release the lock
            return p;
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
                diff = (uintptr_t)(maj) + maj->size;
                diff -= (uintptr_t)min;
                diff -= sizeof(MinorBlock);
                diff -= min->size;
                // minus already existing usage..

                if (diff >= (size + sizeof(MinorBlock)))
                {
                    // yay....
                    min->next = (MinorBlock *)((uintptr_t)min + sizeof(MinorBlock) + min->size);
                    min->next->prev = min;
                    min = min->next;
                    min->next = nullptr;
                    min->magic = LIBALLOC_MAGIC;
                    min->block = maj;
                    min->size = size;
                    min->req_size = req_size;
                    maj->usage += size + sizeof(MinorBlock);

                    void *p = (void *)((uintptr_t)min + sizeof(MinorBlock));
                    __plug_memalloc_unlock(); // release the lock
                    return p;
                }
            }

            // CASE 4.2: Is there space between two minors?
            if (min->next != nullptr)
            {
                // is the difference between here and next big enough?
                diff = (uintptr_t)(min->next);
                diff -= (uintptr_t)min;
                diff -= sizeof(MinorBlock);
                diff -= min->size;
                // minus our existing usage.

                if (diff >= (size + sizeof(MinorBlock)))
                {
                    // yay......
                    MinorBlock *new_min = (MinorBlock *)((uintptr_t)min + sizeof(MinorBlock) + min->size);

                    new_min->magic = LIBALLOC_MAGIC;
                    new_min->next = min->next;
                    new_min->prev = min;
                    new_min->size = size;
                    new_min->req_size = req_size;
                    new_min->block = maj;
                    min->next->prev = new_min;
                    min->next = new_min;
                    maj->usage += size + sizeof(MinorBlock);

                    void *p = (void *)((uintptr_t)new_min + sizeof(MinorBlock));
                    __plug_memalloc_unlock(); // release the lock
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

            if (startedBet == 1)
            {
                maj = l_memRoot;
                startedBet = 0;
                continue;
            }

            // we've run out. we need more...
            maj->next = allocate_new_page(size); // next one guaranteed to be okay
            if (maj->next == nullptr)
                break; //  uh oh,  no more memory.....
            maj->next->prev = maj;
        }

#endif

        maj = maj->next;
    } // while (maj != nullptr)

    __plug_memalloc_unlock(); // release the lock

    logger_warn("All cases exhausted. No memory available.");

    return nullptr;
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
        logger_error("Possible 1-3 byte overrun for magic 0x%x != 0x%x from 0x%x.",
                     min->magic,
                     LIBALLOC_MAGIC,
                     caller);
    }

    if (min->magic == LIBALLOC_DEAD)
    {
        logger_error("Multiple free() attempt on 0x%x from 0x%x.",
                     ptr,
                     caller);
    }
    else
    {
        logger_error("Bad free( 0x%x )  from 0x%x",
                     ptr,
                     caller);
    }

    __plug_memalloc_unlock(); // release the lock

    return false;
}

void free(void *ptr)
{
    MinorBlock *min;
    MajorBlock *maj;

    if (ptr == nullptr)
    {
        logger_warn("free( nullptr ) called from 0x%x", __builtin_return_address(0));
        return;
    }

    __plug_memalloc_lock(); // lockit

    min = (MinorBlock *)((uintptr_t)ptr - sizeof(MinorBlock));

    if (!check_minor_magic(min, ptr, __builtin_return_address(0)))
    {
        return;
    }

    maj = min->block;

    maj->usage -= (min->size + sizeof(MinorBlock));
    min->magic = LIBALLOC_DEAD; // No mojo.

    if (min->next != nullptr)
        min->next->prev = min->prev;
    if (min->prev != nullptr)
        min->prev->next = min->next;

    if (min->prev == nullptr)
        maj->first = min->next;
    // Might empty the block. This was the first
    // minor.

    // We need to clean up after the majors now....

    if (maj->first == nullptr) // Block completely unused.
    {
        if (l_memRoot == maj)
        {
            l_memRoot = maj->next;
        }

        if (l_bestBet == maj)
        {
            l_bestBet = nullptr;
        }

        if (maj->prev != nullptr)
        {
            maj->prev->next = maj->next;
        }

        if (maj->next != nullptr)
        {
            maj->next->prev = maj->prev;
        }

        __plug_memalloc_free(maj, maj->pages * l_pageSize);
    }
    else
    {
        if (l_bestBet != nullptr)
        {
            int bestSize = l_bestBet->size - l_bestBet->usage;
            int majSize = maj->size - maj->usage;

            if (majSize > bestSize)
                l_bestBet = maj;
        }
    }

    __plug_memalloc_unlock(); // release the lock
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

void *realloc(void *p, size_t size)
{
    // Honour the case of size == 0 => free old and return nullptr
    if (size == 0)
    {
        free(p);
        return nullptr;
    }

    // In the case of a nullptr pointer, return a simple malloc.
    if (!p)
    {
        return malloc(size);
    }

    void *ptr = p;
    __plug_memalloc_lock(); // lockit

    MinorBlock *min = (MinorBlock *)((uintptr_t)ptr - sizeof(MinorBlock));

    if (!check_minor_magic(min, ptr, __builtin_return_address(0)))
    {
        return nullptr;
    }

    // Definitely a memory block.
    unsigned int real_size = min->req_size;

    if (real_size >= size)
    {
        min->req_size = size;
        __plug_memalloc_unlock();
        return p;
    }

    __plug_memalloc_unlock();

    // If we got here then we're reallocating to a block bigger than us.
    ptr = malloc(size); // We need to allocate new memory
    memcpy(ptr, p, real_size);
    free(p);

    return ptr;
}
