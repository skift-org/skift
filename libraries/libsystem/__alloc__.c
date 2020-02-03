/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* __alloc__.c : based on durand's Amazing Super Duper Memory functions.      */

#include <libsystem/__plugs__.h>

#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/runtime.h>

#define VERSION "1.1"
#define ALIGNMENT 16ul
#define ALIGN_TYPE char					   ///unsigned char[16] /// unsigned short
#define ALIGN_INFO sizeof(ALIGN_TYPE) * 16 ///< Alignment information is stored right before the pointer. This is the number of bytes of information stored there.

#define PREFIX(func) func

#define LIBALLOC_MAGIC 0xc001c0de
#define LIBALLOC_DEAD 0xdeaddead

#define USE_CASE1
#define USE_CASE2
#define USE_CASE3
#define USE_CASE4
#define USE_CASE5

#define INFO

/** This macro will conveniently align our pointer upwards */
#define ALIGN(ptr)                                       \
	if (ALIGNMENT > 1)                                   \
	{                                                    \
		uintptr_t diff;                                  \
		ptr = (void *)((uintptr_t)ptr + ALIGN_INFO);     \
		diff = (uintptr_t)ptr & (ALIGNMENT - 1);         \
		if (diff != 0)                                   \
		{                                                \
			diff = ALIGNMENT - diff;                     \
			ptr = (void *)((uintptr_t)ptr + diff);       \
		}                                                \
		*((ALIGN_TYPE *)((uintptr_t)ptr - ALIGN_INFO)) = \
			diff + ALIGN_INFO;                           \
	}

#define UNALIGN(ptr)                                                     \
	if (ALIGNMENT > 1)                                                   \
	{                                                                    \
		uintptr_t diff = *((ALIGN_TYPE *)((uintptr_t)ptr - ALIGN_INFO)); \
		if (diff < (ALIGNMENT + ALIGN_INFO))                             \
		{                                                                \
			ptr = (void *)((uintptr_t)ptr - diff);                       \
		}                                                                \
	}

#if defined DEBUG || defined INFO
#define FLUSH() stream_flush(out_stream)
#endif

/** A structure found at the top of all system allocated 
 * memory blocks. It details the usage of the memory block.
 */
struct liballoc_major
{
	struct liballoc_major *prev;  ///< Linked list information.
	struct liballoc_major *next;  ///< Linked list information.
	unsigned int pages;			  ///< The number of pages in the block.
	unsigned int size;			  ///< The number of pages in the block.
	unsigned int usage;			  ///< The number of bytes used in the block.
	struct liballoc_minor *first; ///< A pointer to the first allocated memory in the block.
};

/** This is a structure found at the beginning of all
 * sections in a major block which were allocated by a
 * malloc, calloc, realloc call.
 */
struct liballoc_minor
{
	unsigned int magic;			  ///< A magic number to idenfity correctness.
	unsigned int size;			  ///< The size of the memory allocated. Could be 1 byte or more.
	unsigned int req_size;		  ///< The size of memory requested.
	struct liballoc_minor *prev;  ///< Linked list information.
	struct liballoc_minor *next;  ///< Linked list information.
	struct liballoc_major *block; ///< The owning block. A pointer to the major structure.
};

static struct liballoc_major *l_memRoot = NULL; ///< The root memory block acquired from the system.
static struct liballoc_major *l_bestBet = NULL; ///< The major with the most free memory.

static unsigned int l_pageSize = 4096;	 ///< The size of an individual page. Set up in liballoc_init.
static unsigned int l_pageCount = 16;	  ///< The number of pages to request per chunk. Set up in liballoc_init.
static unsigned long long l_allocated = 0; ///< Running total of allocated memory.
static unsigned long long l_inuse = 0;	 ///< Running total of used memory.

static long long l_warningCount = 0;	 ///< Number of warnings encountered
static long long l_errorCount = 0;		 ///< Number of actual errors
static long long l_possibleOverruns = 0; ///< Number of possible overruns

// ***********   HELPER FUNCTIONS  *******************************

#if defined DEBUG
static void liballoc_dump()
{
#ifdef DEBUG
	struct liballoc_major *maj = l_memRoot;
	struct liballoc_minor *min = NULL;
#endif

	stream_printf(log_stream, "liballoc: ------ Memory data ---------------\n");
	stream_printf(log_stream, "liballoc: System memory allocated: %i bytes\n", l_allocated);
	stream_printf(log_stream, "liballoc: Memory in used (malloc'ed): %i bytes\n", l_inuse);
	stream_printf(log_stream, "liballoc: Warning count: %i\n", l_warningCount);
	stream_printf(log_stream, "liballoc: Error count: %i\n", l_errorCount);
	stream_printf(log_stream, "liballoc: Possible overruns: %i\n", l_possibleOverruns);

#ifdef DEBUG
	while (maj != NULL)
	{
		stream_printf(log_stream, "liballoc: 0x%x: total = %i, used = %i\n",
						maj,
						maj->size,
						maj->usage);

		min = maj->first;
		while (min != NULL)
		{
			stream_printf(log_stream, "liballoc:    0x%x: %i bytes\n",
							min,
							min->size);
			min = min->next;
		}

		maj = maj->next;
	}
#endif

	FLUSH();
}
#endif

// ***************************************************************

static struct liballoc_major *allocate_new_page(unsigned int size)
{
	unsigned int st;
	struct liballoc_major *maj;

	// This is how much space is required.
	st = size + sizeof(struct liballoc_major);
	st += sizeof(struct liballoc_minor);

	// Perfect amount of space?
	if ((st % l_pageSize) == 0)
		st = st / (l_pageSize);
	else
		st = st / (l_pageSize) + 1;
	// No, add the buffer.

	// Make sure it's >= the minimum size.
	if (st < l_pageCount)
		st = l_pageCount;

	maj = (struct liballoc_major *)__plug_memalloc_alloc(st);

	if (maj == NULL)
	{
		l_warningCount += 1;
#if defined DEBUG || defined INFO
		logger_warn("__plug_memalloc_alloc( %i ) return NULL", st);
#endif
		return NULL; // uh oh, we ran out of memory.
	}

	maj->prev = NULL;
	maj->next = NULL;
	maj->pages = st;
	maj->size = st * l_pageSize;
	maj->usage = sizeof(struct liballoc_major);
	maj->first = NULL;

	l_allocated += maj->size;

#ifdef DEBUG
	stream_printf(log_stream, "liballoc: Resource allocated 0x%x of %i pages (%i bytes) for %i size.\n", maj, st, maj->size, size);

	stream_printf(log_stream, "liballoc: Total memory usage = %i KB\n", (int)((l_allocated / (1024))));
	FLUSH();
#endif

	return maj;
}

void *malloc(size_t req_size)
{
	int startedBet = 0;
	unsigned long long bestSize = 0;
	void *p = NULL;
	uintptr_t diff;
	struct liballoc_major *maj;
	struct liballoc_minor *min;
	struct liballoc_minor *new_min;
	unsigned long size = req_size;

	// For alignment, we adjust size so there's enough space to align.
	if (ALIGNMENT > 1)
	{
		size += ALIGNMENT + ALIGN_INFO;
	}

	// So, ideally, we really want an alignment of 0 or 1 in order
	// to save space.

	__plug_memalloc_lock();

	if (size == 0)
	{
		l_warningCount += 1;

		logger_warn("alloc( 0 ) called from 0x%x", __builtin_return_address(0));
		__plug_memalloc_unlock();

		return malloc(1);
	}

	if (l_memRoot == NULL)
	{
#if defined DEBUG || defined INFO
#ifdef DEBUG
		stream_printf(log_stream, "liballoc: initialization of liballoc " VERSION "\n");
#endif
		FLUSH();
#endif

		// This is the first time we are being used.
		l_memRoot = allocate_new_page(size);
		if (l_memRoot == NULL)
		{
			__plug_memalloc_unlock();
#ifdef DEBUG
			stream_printf(log_stream, "liballoc: initial l_memRoot initialization failed\n", p);
			FLUSH();
#endif
			return NULL;
		}

#ifdef DEBUG
		stream_printf(log_stream, "liballoc: set up first memory major 0x%x\n", l_memRoot);
		FLUSH();
#endif
	}

#ifdef DEBUG
	stream_printf(log_stream, "liballoc: 0x%x malloc( %i ): ",
					__builtin_return_address(0),
					size);
	FLUSH();
#endif

	// Now we need to bounce through every major and find enough space....

	maj = l_memRoot;
	startedBet = 0;

	// Start at the best bet....
	if (l_bestBet != NULL)
	{
		bestSize = l_bestBet->size - l_bestBet->usage;

		if (bestSize > (size + sizeof(struct liballoc_minor)))
		{
			maj = l_bestBet;
			startedBet = 1;
		}
	}

	while (maj != NULL)
	{
		diff = maj->size - maj->usage;
		// free memory in the block

		if (bestSize < diff)
		{
			// Hmm.. this one has more memory then our bestBet. Remember!
			l_bestBet = maj;
			bestSize = diff;
		}

#ifdef USE_CASE1

		// CASE 1:  There is not enough space in this major block.
		if (diff < (size + sizeof(struct liballoc_minor)))
		{
#ifdef DEBUG
			stream_printf(log_stream, "CASE 1: Insufficient space in block 0x%x\n", maj);
			FLUSH();
#endif

			// Another major block next to this one?
			if (maj->next != NULL)
			{
				maj = maj->next; // Hop to that one.
				continue;
			}

			if (startedBet == 1) // If we started at the best bet,
			{					 // let's start all over again.
				maj = l_memRoot;
				startedBet = 0;
				continue;
			}

			// Create a new major block next to this one and...
			maj->next = allocate_new_page(size); // next one will be okay.
			if (maj->next == NULL)
				break; // no more memory.
			maj->next->prev = maj;
			maj = maj->next;

			// .. fall through to CASE 2 ..
		}

#endif

#ifdef USE_CASE2

		// CASE 2: It's a brand new block.
		if (maj->first == NULL)
		{
			maj->first = (struct liballoc_minor *)((uintptr_t)maj + sizeof(struct liballoc_major));

			maj->first->magic = LIBALLOC_MAGIC;
			maj->first->prev = NULL;
			maj->first->next = NULL;
			maj->first->block = maj;
			maj->first->size = size;
			maj->first->req_size = req_size;
			maj->usage += size + sizeof(struct liballoc_minor);

			l_inuse += size;

			p = (void *)((uintptr_t)(maj->first) + sizeof(struct liballoc_minor));

			ALIGN(p);

#ifdef DEBUG
			stream_printf(log_stream, "CASE 2: returning 0x%x\n", p);
			FLUSH();
#endif
			__plug_memalloc_unlock(); // release the lock
			return p;
		}

#endif

#ifdef USE_CASE3

		// CASE 3: Block in use and enough space at the start of the block.
		diff = (uintptr_t)(maj->first);
		diff -= (uintptr_t)maj;
		diff -= sizeof(struct liballoc_major);

		if (diff >= (size + sizeof(struct liballoc_minor)))
		{
			// Yes, space in front. Squeeze in.
			maj->first->prev = (struct liballoc_minor *)((uintptr_t)maj + sizeof(struct liballoc_major));
			maj->first->prev->next = maj->first;
			maj->first = maj->first->prev;

			maj->first->magic = LIBALLOC_MAGIC;
			maj->first->prev = NULL;
			maj->first->block = maj;
			maj->first->size = size;
			maj->first->req_size = req_size;
			maj->usage += size + sizeof(struct liballoc_minor);

			l_inuse += size;

			p = (void *)((uintptr_t)(maj->first) + sizeof(struct liballoc_minor));
			ALIGN(p);

#ifdef DEBUG
			stream_printf(log_stream, "CASE 3: returning 0x%x\n", p);
			FLUSH();
#endif
			__plug_memalloc_unlock(); // release the lock
			return p;
		}

#endif

#ifdef USE_CASE4

		// CASE 4: There is enough space in this block. But is it contiguous?
		min = maj->first;

		// Looping within the block now...
		while (min != NULL)
		{
			// CASE 4.1: End of minors in a block. Space from last and end?
			if (min->next == NULL)
			{
				// the rest of this block is free...  is it big enough?
				diff = (uintptr_t)(maj) + maj->size;
				diff -= (uintptr_t)min;
				diff -= sizeof(struct liballoc_minor);
				diff -= min->size;
				// minus already existing usage..

				if (diff >= (size + sizeof(struct liballoc_minor)))
				{
					// yay....
					min->next = (struct liballoc_minor *)((uintptr_t)min + sizeof(struct liballoc_minor) + min->size);
					min->next->prev = min;
					min = min->next;
					min->next = NULL;
					min->magic = LIBALLOC_MAGIC;
					min->block = maj;
					min->size = size;
					min->req_size = req_size;
					maj->usage += size + sizeof(struct liballoc_minor);

					l_inuse += size;

					p = (void *)((uintptr_t)min + sizeof(struct liballoc_minor));
					ALIGN(p);

#ifdef DEBUG
					stream_printf(log_stream, "CASE 4.1: returning 0x%x\n", p);
					FLUSH();
#endif
					__plug_memalloc_unlock(); // release the lock
					return p;
				}
			}

			// CASE 4.2: Is there space between two minors?
			if (min->next != NULL)
			{
				// is the difference between here and next big enough?
				diff = (uintptr_t)(min->next);
				diff -= (uintptr_t)min;
				diff -= sizeof(struct liballoc_minor);
				diff -= min->size;
				// minus our existing usage.

				if (diff >= (size + sizeof(struct liballoc_minor)))
				{
					// yay......
					new_min = (struct liballoc_minor *)((uintptr_t)min + sizeof(struct liballoc_minor) + min->size);

					new_min->magic = LIBALLOC_MAGIC;
					new_min->next = min->next;
					new_min->prev = min;
					new_min->size = size;
					new_min->req_size = req_size;
					new_min->block = maj;
					min->next->prev = new_min;
					min->next = new_min;
					maj->usage += size + sizeof(struct liballoc_minor);

					l_inuse += size;

					p = (void *)((uintptr_t)new_min + sizeof(struct liballoc_minor));
					ALIGN(p);

#ifdef DEBUG
					stream_printf(log_stream, "CASE 4.2: returning 0x%x\n", p);
					FLUSH();
#endif

					__plug_memalloc_unlock(); // release the lock
					return p;
				}
			} // min->next != NULL

			min = min->next;
		} // while min != NULL ...

#endif

#ifdef USE_CASE5

		// CASE 5: Block full! Ensure next block and loop.
		if (maj->next == NULL)
		{
#ifdef DEBUG
			stream_printf(log_stream, "CASE 5: block full\n");
			FLUSH();
#endif

			if (startedBet == 1)
			{
				maj = l_memRoot;
				startedBet = 0;
				continue;
			}

			// we've run out. we need more...
			maj->next = allocate_new_page(size); // next one guaranteed to be okay
			if (maj->next == NULL)
				break; //  uh oh,  no more memory.....
			maj->next->prev = maj;
		}

#endif

		maj = maj->next;
	} // while (maj != NULL)

	__plug_memalloc_unlock(); // release the lock

	logger_warn("All cases exhausted. No memory available.");

	return NULL;
}

void free(void *ptr)
{
	struct liballoc_minor *min;
	struct liballoc_major *maj;

	if (ptr == NULL)
	{
		l_warningCount += 1;
#if defined DEBUG || defined INFO
		logger_warn("free( NULL ) called from 0x%x",
					__builtin_return_address(0));
		FLUSH();
#endif
		return;
	}

	UNALIGN(ptr);

	__plug_memalloc_lock(); // lockit

	min = (struct liballoc_minor *)((uintptr_t)ptr - sizeof(struct liballoc_minor));

	if (min->magic != LIBALLOC_MAGIC)
	{
		l_errorCount += 1;

		// Check for overrun errors. For all bytes of LIBALLOC_MAGIC
		if (((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) ||
			((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) ||
			((min->magic & 0xFF) == (LIBALLOC_MAGIC & 0xFF)))
		{
			l_possibleOverruns += 1;
			logger_error("Possible 1-3 byte overrun for magic 0x%x != 0x%x",
						 min->magic,
						 LIBALLOC_MAGIC);
		}

		if (min->magic == LIBALLOC_DEAD)
		{
			logger_error("Multiple free() attempt on 0x%x from 0x%x.",
						 ptr,
						 __builtin_return_address(0));
		}
		else
		{
			logger_error("Bad free( 0x%x ) called from 0x%x",
						 ptr,
						 __builtin_return_address(0));
		}

		// being lied to...
		__plug_memalloc_unlock(); // release the lock
		return;
	}

#ifdef DEBUG
	stream_printf(log_stream, "liballoc: 0x%x free( 0x%x ): ",
					__builtin_return_address(0),
					ptr);
	FLUSH();
#endif

	maj = min->block;

	l_inuse -= min->size;

	maj->usage -= (min->size + sizeof(struct liballoc_minor));
	min->magic = LIBALLOC_DEAD; // No mojo.

	if (min->next != NULL)
		min->next->prev = min->prev;
	if (min->prev != NULL)
		min->prev->next = min->next;

	if (min->prev == NULL)
		maj->first = min->next;
	// Might empty the block. This was the first
	// minor.

	// We need to clean up after the majors now....

	if (maj->first == NULL) // Block completely unused.
	{
		if (l_memRoot == maj)
			l_memRoot = maj->next;
		if (l_bestBet == maj)
			l_bestBet = NULL;
		if (maj->prev != NULL)
			maj->prev->next = maj->next;
		if (maj->next != NULL)
			maj->next->prev = maj->prev;
		l_allocated -= maj->size;

		__plug_memalloc_free(maj, maj->pages);
	}
	else
	{
		if (l_bestBet != NULL)
		{
			int bestSize = l_bestBet->size - l_bestBet->usage;
			int majSize = maj->size - maj->usage;

			if (majSize > bestSize)
				l_bestBet = maj;
		}
	}

#ifdef DEBUG
	stream_printf(log_stream, "OK\n");
	FLUSH();
#endif

	__plug_memalloc_unlock(); // release the lock
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
	// Honour the case of size == 0 => free old and return NULL
	if (size == 0)
	{
		free(p);
		return NULL;
	}

	// In the case of a NULL pointer, return a simple malloc.
	if (p == NULL)
		return malloc(size);

	// Unalign the pointer if required.
	void *ptr = p;
	UNALIGN(ptr);

	__plug_memalloc_lock(); // lockit

	struct liballoc_minor *min = (struct liballoc_minor *)((uintptr_t)ptr - sizeof(struct liballoc_minor));

	// Ensure it is a valid structure.
	if (min->magic != LIBALLOC_MAGIC)
	{
		l_errorCount += 1;

		// Check for overrun errors. For all bytes of LIBALLOC_MAGIC
		if (((min->magic & 0xFFFFFF) == (LIBALLOC_MAGIC & 0xFFFFFF)) ||
			((min->magic & 0xFFFF) == (LIBALLOC_MAGIC & 0xFFFF)) ||
			((min->magic & 0xFF) == (LIBALLOC_MAGIC & 0xFF)))
		{
			l_possibleOverruns += 1;
			logger_error("Possible 1-3 byte overrun for magic 0x%x != 0x%x",
						 min->magic,
						 LIBALLOC_MAGIC);
		}

		if (min->magic == LIBALLOC_DEAD)
		{
			logger_error("Multiple free() attempt on 0x%x from 0x%x.",
						 ptr,
						 __builtin_return_address(0));
		}
		else
		{
			logger_error("Bad free( 0x%x ) called from 0x%x",
						 ptr,
						 __builtin_return_address(0));
		}

		// being lied to...
		__plug_memalloc_unlock(); // release the lock
		return NULL;
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
