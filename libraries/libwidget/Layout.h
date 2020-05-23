#define LAY_IMPLEMENTATION

#ifndef LAY_INCLUDE_HEADER
#define LAY_INCLUDE_HEADER

// Do this:
//
//   #define LAY_IMPLEMENTATION
//
// in exactly one C or C++ file in your project before you include layout.h.
// Your includes should look like this:
//
//   #include ...
//   #include ...
//   #define LAY_IMPLEMENTATION
//   #include "layout.h"
//
// All other files in your project should not define LAY_IMPLEMENTATION.

#include <stdint.h>

#ifndef LAY_EXPORT
#define LAY_EXPORT extern
#endif

// Users of this library can define LAY_ASSERT if they would like to use an
// assert other than the one from assert.h.
#ifndef LAY_ASSERT
#include <assert.h>
#define LAY_ASSERT assert
#endif

// 'static inline' for things we always want inlined -- the compiler should not
// even have to consider not inlining these.
#if defined(__GNUC__) || defined(__clang__)
#define LAY_STATIC_INLINE __attribute__((always_inline)) static inline
#elif defined(_MSC_VER)
#define LAY_STATIC_INLINE __forceinline static
#else
#define LAY_STATIC_INLINE inline static
#endif

typedef uint32_t lay_id;
#if LAY_FLOAT == 1
typedef float lay_scalar;
#else
typedef int16_t lay_scalar;
#endif

#define LAY_INVALID_ID UINT32_MAX

// GCC and Clang allow us to create vectors based on a type with the
// vector_size extension. This will allow us to access individual components of
// the vector via indexing operations.
#if defined(__GNUC__) || defined(__clang__)

// Using floats for coordinates takes up more space than using int16. 128 bits
// for a four-component vector.
#ifdef LAY_FLOAT
typedef float lay_vec4 __attribute__((__vector_size__(16), aligned(4)));
typedef float lay_vec2 __attribute__((__vector_size__(8), aligned(4)));
// Integer version uses 64 bits for a four-component vector.
#else
typedef int16_t lay_vec4 __attribute__((__vector_size__(8), aligned(2)));
typedef int16_t lay_vec2 __attribute__((__vector_size__(4), aligned(2)));
#endif // LAY_FLOAT

// Note that we're not actually going to make any explicit use of any
// platform's SIMD instructions -- we're just using the vector extension for
// more convenient syntax. Therefore, we can specify more relaxed alignment
// requirements. See the end of this file for some notes about this.

// MSVC doesn't have the vetor_size attribute, but we want convenient indexing
// operators for our layout logic code. Therefore, we force C++ compilation in
// MSVC, and use C++ operator overloading.
#elif defined(_MSC_VER)
struct lay_vec4
{
    lay_scalar xyzw[4];
    const lay_scalar &operator[](int index) const
    {
        return xyzw[index];
    }
    lay_scalar &operator[](int index)
    {
        return xyzw[index];
    }
};
struct lay_vec2
{
    lay_scalar xy[2];
    const lay_scalar &operator[](int index) const
    {
        return xy[index];
    }
    lay_scalar &operator[](int index)
    {
        return xy[index];
    }
};
#endif // __GNUC__/__clang__ or _MSC_VER

typedef struct lay_item_t
{
    uint32_t flags;
    lay_id first_child;
    lay_id next_sibling;
    lay_vec4 margins;
    lay_vec2 size;
} lay_item_t;

typedef struct lay_context
{
    lay_item_t *items;
    lay_vec4 *rects;
    lay_id capacity;
    lay_id count;
} lay_context;

// Container flags to pass to lay_set_container()
typedef enum lay_box_flags
{
    // flex-direction (bit 0+1)

    // left to right
    LAY_ROW = 0x002,
    // top to bottom
    LAY_COLUMN = 0x003,

    // model (bit 1)

    // free layout
    LAY_LAYOUT = 0x000,
    // flex model
    LAY_FLEX = 0x002,

    // flex-wrap (bit 2)

    // single-line
    LAY_NOWRAP = 0x000,
    // multi-line, wrap left to right
    LAY_WRAP = 0x004,

    // justify-content (start, end, center, space-between)
    // at start of row/column
    LAY_START = 0x008,
    // at center of row/column
    LAY_MIDDLE = 0x000,
    // at end of row/column
    LAY_END = 0x010,
    // insert spacing to stretch across whole row/column
    LAY_JUSTIFY = 0x018,

    // align-items
    // can be implemented by putting a flex container in a layout container,
    // then using LAY_TOP, LAY_BOTTOM, LAY_VFILL, LAY_VCENTER, etc.
    // FILL is equivalent to stretch/grow

    // align-content (start, end, center, stretch)
    // can be implemented by putting a flex container in a layout container,
    // then using LAY_TOP, LAY_BOTTOM, LAY_VFILL, LAY_VCENTER, etc.
    // FILL is equivalent to stretch; space-between is not supported.
} lay_box_flags;

// child layout flags to pass to lay_set_behave()
typedef enum lay_layout_flags
{
    // attachments (bit 5-8)
    // fully valid when parent uses LAY_LAYOUT model
    // partially valid when in LAY_FLEX model

    // anchor to left item or left side of parent
    LAY_LEFT = 0x020,
    // anchor to top item or top side of parent
    LAY_TOP = 0x040,
    // anchor to right item or right side of parent
    LAY_RIGHT = 0x080,
    // anchor to bottom item or bottom side of parent
    LAY_BOTTOM = 0x100,
    // anchor to both left and right item or parent borders
    LAY_HFILL = 0x0a0,
    // anchor to both top and bottom item or parent borders
    LAY_VFILL = 0x140,
    // center horizontally, with left margin as offset
    LAY_HCENTER = 0x000,
    // center vertically, with top margin as offset
    LAY_VCENTER = 0x000,
    // center in both directions, with left/top margin as offset
    LAY_CENTER = 0x000,
    // anchor to all four directions
    LAY_FILL = 0x1e0,
    // When in a wrapping container, put this element on a new line. Wrapping
    // layout code auto-inserts LAY_BREAK flags as needed. See GitHub issues for
    // TODO related to this.
    //
    // Drawing routines can read this via item pointers as needed after
    // performing layout calculations.
    LAY_BREAK = 0x200
} lay_layout_flags;

enum
{
    // these bits, starting at bit 16, can be safely assigned by the
    // application, e.g. as item types, other event types, drop targets, etc.
    // this is not yet exposed via API functions, you'll need to get/set these
    // by directly accessing item pointers.
    //
    // (In reality we have more free bits than this, TODO)
    //
    // TODO fix int/unsigned size mismatch (clang issues warning for this),
    // should be all bits as 1 instead of INT_MAX
    LAY_USERMASK = 0x7fff0000,

    // a special mask passed to lay_find_item() (currently does not exist, was
    // not ported from oui)
    LAY_ANY = 0x7fffffff,
};

enum
{
    // extra item flags

    // bit 0-2
    LAY_ITEM_BOX_MODEL_MASK = 0x000007,
    // bit 0-4
    LAY_ITEM_BOX_MASK = 0x00001F,
    // bit 5-9
    LAY_ITEM_LAYOUT_MASK = 0x0003E0,
    // item has been inserted (bit 10)
    LAY_ITEM_INSERTED = 0x400,
    // horizontal size has been explicitly set (bit 11)
    LAY_ITEM_HFIXED = 0x800,
    // vertical size has been explicitly set (bit 12)
    LAY_ITEM_VFIXED = 0x1000,
    // bit 11-12
    LAY_ITEM_FIXED_MASK = LAY_ITEM_HFIXED | LAY_ITEM_VFIXED,

    // which flag bits will be compared
    LAY_ITEM_COMPARE_MASK = LAY_ITEM_BOX_MODEL_MASK | (LAY_ITEM_LAYOUT_MASK & ~LAY_BREAK) | LAY_USERMASK,
};

LAY_STATIC_INLINE lay_vec4 lay_vec4_xyzw(lay_scalar x, lay_scalar y, lay_scalar z, lay_scalar w)
{
#if defined(__GNUC__) || defined(__clang__)
    return (lay_vec4){x, y, z, w};
#elif defined(_MSC_VER)
    lay_vec4 result;
    result[0] = x;
    result[1] = y;
    result[2] = z;
    result[3] = w;
    return result;
#endif
}

// Call this on a context before using it. You must also call this on a context
// if you would like to use it again after calling lay_destroy_context() on it.
LAY_EXPORT void lay_init_context(lay_context *ctx);

// Reserve enough heap memory to contain `count` items without needing to
// reallocate. The initial lay_init_context() call does not allocate any heap
// memory, so if you init a context and then call this once with a large enough
// number for the number of items you'll create, there will not be any further
// reallocations.
LAY_EXPORT void lay_reserve_items_capacity(lay_context *ctx, lay_id count);

// Frees any heap allocated memory used by a context. Don't call this on a
// context that did not have lay_init_context() call on it. To reuse a context
// after destroying it, you will need to call lay_init_context() on it again.
LAY_EXPORT void lay_destroy_context(lay_context *ctx);

// Clears all of the items in a context, setting its count to 0. Use this when
// you want to re-declare your layout starting from the root item. This does not
// free any memory or perform allocations. It's safe to use the context again
// after calling this. You should probably use this instead of init/destroy if
// you are recalculating your layouts in a loop.
LAY_EXPORT void lay_reset_context(lay_context *ctx);

// Performs the layout calculations, starting at the root item (id 0). After
// calling this, you can use lay_get_rect() to query for an item's calculated
// rectangle. If you use procedures such as lay_append() or lay_insert() after
// calling this, your calculated data may become invalid if a reallocation
// occurs.
//
// You should prefer to recreate your items starting from the root instead of
// doing fine-grained updates to the existing context.
//
// However, it's safe to use lay_set_size on an item, and then re-run
// lay_run_context. This might be useful if you are doing a resizing animation
// on items in a layout without any contents changing.
LAY_EXPORT void lay_run_context(lay_context *ctx);

// Like lay_run_context(), this procedure will run layout calculations --
// however, it lets you specify which item you want to start from.
// lay_run_context() always starts with item 0, the first item, as the root.
// Running the layout calculations from a specific item is useful if you want
// need to iteratively re-run parts of your layout hierarchy, or if you are only
// interested in updating certain subsets of it. Be careful when using this --
// it's easy to generated bad output if the parent items haven't yet had their
// output rectangles calculated, or if they've been invalidated (e.g. due to
// re-allocation).
LAY_EXPORT void lay_run_item(lay_context *ctx, lay_id item);

// Performing a layout on items where wrapping is enabled in the parent
// container can cause flags to be modified during the calculations. If you plan
// to call lay_run_context or lay_run_item multiple times without calling
// lay_reset, and if you have a container that uses wrapping, and if the width
// or height of the container may have changed, you should call
// lay_clear_item_break on all of the children of a container before calling
// lay_run_context or lay_run_item again. If you don't, the layout calculations
// may perform unnecessary wrapping.
//
// This requirement may be changed in the future.
//
// Calling this will also reset any manually-specified breaking. You will need
// to set the manual breaking again, or simply not call this on any items that
// you know you wanted to break manually.
//
// If you clear your context every time you calculate your layout, or if you
// don't use wrapping, you don't need to call this.
LAY_EXPORT void lay_clear_item_break(lay_context *ctx, lay_id item);

// Returns the number of items that have been created in a context.
LAY_EXPORT lay_id lay_items_count(lay_context *ctx);

// Returns the number of items the context can hold without performing a
// reallocation.
LAY_EXPORT lay_id lay_items_capacity(lay_context *ctx);

// Create a new item, which can just be thought of as a rectangle. Returns the
// id (handle) used to identify the item.
LAY_EXPORT lay_id lay_item(lay_context *ctx);

// Inserts an item into another item, forming a parent - child relationship. An
// item can contain any number of child items. Items inserted into a parent are
// put at the end of the ordering, after any existing siblings.
LAY_EXPORT void lay_insert(lay_context *ctx, lay_id parent, lay_id child);

// lay_append inserts an item as a sibling after another item. This allows
// inserting an item into the middle of an existing list of items within a
// parent. It's also more efficient than repeatedly using lay_insert(ctx,
// parent, new_child) in a loop to create a list of items in a parent, because
// it does not need to traverse the parent's children each time. So if you're
// creating a long list of children inside of a parent, you might prefer to use
// this after using lay_insert to insert the first child.
LAY_EXPORT void lay_append(lay_context *ctx, lay_id earlier, lay_id later);

// Like lay_insert, but puts the new item as the first child in a parent instead
// of as the last.
LAY_EXPORT void lay_push(lay_context *ctx, lay_id parent, lay_id child);

// Gets the size that was set with lay_set_size or lay_set_size_xy. The _xy
// version writes the output values to the specified addresses instead of
// returning the values in a lay_vec2.
LAY_EXPORT lay_vec2 lay_get_size(lay_context *ctx, lay_id item);
LAY_EXPORT void lay_get_size_xy(lay_context *ctx, lay_id item, lay_scalar *x, lay_scalar *y);

// Sets the size of an item. The _xy version passes the width and height as
// separate arguments, but functions the same.
LAY_EXPORT void lay_set_size(lay_context *ctx, lay_id item, lay_vec2 size);
LAY_EXPORT void lay_set_size_xy(lay_context *ctx, lay_id item, lay_scalar width, lay_scalar height);

// Set the flags on an item which determines how it behaves as a parent. For
// example, setting LAY_COLUMN will make an item behave as if it were a column
// -- it will lay out its children vertically.
LAY_EXPORT void lay_set_contain(lay_context *ctx, lay_id item, uint32_t flags);

// Set the flags on an item which determines how it behaves as a child inside of
// a parent item. For example, setting LAY_VFILL will make an item try to fill
// up all available vertical space inside of its parent.
LAY_EXPORT void lay_set_behave(lay_context *ctx, lay_id item, uint32_t flags);

// Get the margins that were set by lay_set_margins. The _ltrb version writes
// the output values to the specified addresses instead of returning the values
// in a lay_vec4.
// l: left, t: top, r: right, b: bottom
LAY_EXPORT lay_vec4 lay_get_margins(lay_context *ctx, lay_id item);
LAY_EXPORT void lay_get_margins_ltrb(lay_context *ctx, lay_id item, lay_scalar *l, lay_scalar *t, lay_scalar *r, lay_scalar *b);

// Set the margins on an item. The components of the vector are:
// 0: left, 1: top, 2: right, 3: bottom.
LAY_EXPORT void lay_set_margins(lay_context *ctx, lay_id item, lay_vec4 ltrb);

// Same as lay_set_margins, but the components are passed as separate arguments
// (left, top, right, bottom).
LAY_EXPORT void lay_set_margins_ltrb(lay_context *ctx, lay_id item, lay_scalar l, lay_scalar t, lay_scalar r, lay_scalar b);

// Get the pointer to an item in the buffer by its id. Don't keep this around --
// it will become invalid as soon as any reallocation occurs. Just store the id
// instead (it's smaller, anyway, and the lookup cost will be nothing.)
LAY_STATIC_INLINE lay_item_t *lay_get_item(const lay_context *ctx, lay_id id)
{
    LAY_ASSERT(id != LAY_INVALID_ID && id < ctx->count);
    return ctx->items + id;
}

// Get the id of first child of an item, if any. Returns LAY_INVALID_ID if there
// is no child.
LAY_STATIC_INLINE lay_id lay_first_child(const lay_context *ctx, lay_id id)
{
    const lay_item_t *pitem = lay_get_item(ctx, id);
    return pitem->first_child;
}

// Get the id of the next sibling of an item, if any. Returns LAY_INVALID_ID if
// there is no next sibling.
LAY_STATIC_INLINE lay_id lay_next_sibling(const lay_context *ctx, lay_id id)
{
    const lay_item_t *pitem = lay_get_item(ctx, id);
    return pitem->next_sibling;
}

// Returns the calculated rectangle of an item. This is only valid after calling
// lay_run_context and before any other reallocation occurs. Otherwise, the
// result will be undefined. The vector components are:
// 0: x starting position, 1: y starting position
// 2: width, 3: height
LAY_STATIC_INLINE lay_vec4 lay_get_rect(const lay_context *ctx, lay_id id)
{
    LAY_ASSERT(id != LAY_INVALID_ID && id < ctx->count);
    return ctx->rects[id];
}

// The same as lay_get_rect, but writes the x,y positions and width,height
// values to the specified addresses instead of returning them in a lay_vec4.
LAY_STATIC_INLINE void lay_get_rect_xywh(
    const lay_context *ctx, lay_id id,
    lay_scalar *x, lay_scalar *y, lay_scalar *width, lay_scalar *height)
{
    LAY_ASSERT(id != LAY_INVALID_ID && id < ctx->count);
    lay_vec4 rect = ctx->rects[id];
    *x = rect[0];
    *y = rect[1];
    *width = rect[2];
    *height = rect[3];
}

#undef LAY_EXPORT
#undef LAY_STATIC_INLINE

#endif // LAY_INCLUDE_HEADER

// Notes about the use of vector_size merely for syntax convenience:
//
// The current layout calculation procedures are not written in a way that
// would benefit from SIMD instruction usage.
//
// (Passing 128-bit float4 vectors using __vectorcall *might* get you some
// small benefit in very specific situations, but is unlikely to be worth the
// hassle. And I believe this would only be needed if you compiled the library
// in a way where the compiler was prevented from using inlining when copying
// rectangle/size data.)
//
// I might go back in the future and just use regular struct-wrapped arrays.
// I'm not sure if relying the vector thing in GCC/clang and then using C++
// operator overloading in MSVC is worth the annoyance of saving a couple of
// extra characters on each array access in the implementation code.

#ifdef LAY_IMPLEMENTATION

#include <stdbool.h>
#include <stddef.h>

// Users of this library can define LAY_REALLOC to use a custom (re)allocator
// instead of stdlib's realloc. It should have the same behavior as realloc --
// first parameter type is a void pointer, and its value is either a null
// pointer or an existing pointer. The second parameter is a size_t of the new
// desired size. The buffer contents should be preserved across reallocations.
//
// And, if you define LAY_REALLOC, you will also need to define LAY_FREE, which
// should have the same behavior as free.
#ifndef LAY_REALLOC
#include <stdlib.h>
#define LAY_REALLOC(_block, _size) realloc(_block, _size)
#define LAY_FREE(_block) free(_block)
#endif

// Like the LAY_REALLOC define, LAY_MEMSET can be used for a custom memset.
// Otherwise, the memset from string.h will be used.
#ifndef LAY_MEMSET
#include <string.h>
#define LAY_MEMSET(_dst, _val, _size) memset(_dst, _val, _size)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define LAY_FORCE_INLINE __attribute__((always_inline)) inline
#ifdef __cplusplus
#define LAY_RESTRICT __restrict
#else
#define LAY_RESTRICT restrict
#endif // __cplusplus
#elif defined(_MSC_VER)
#define LAY_FORCE_INLINE __forceinline
#define LAY_RESTRICT __restrict
#else
#define LAY_FORCE_INLINE inline
#ifdef __cplusplus
#define LAY_RESTRICT
#else
#define LAY_RESTRICT restrict
#endif // __cplusplus
#endif

// Useful math utilities
static LAY_FORCE_INLINE lay_scalar lay_scalar_max(lay_scalar a, lay_scalar b)
{
    return a > b ? a : b;
}
static LAY_FORCE_INLINE lay_scalar lay_scalar_min(lay_scalar a, lay_scalar b)
{
    return a < b ? a : b;
}
static LAY_FORCE_INLINE float lay_float_max(float a, float b)
{
    return a > b ? a : b;
}
static LAY_FORCE_INLINE float lay_float_min(float a, float b)
{
    return a < b ? a : b;
}

void lay_init_context(lay_context *ctx)
{
    ctx->capacity = 0;
    ctx->count = 0;
    ctx->items = NULL;
    ctx->rects = NULL;
}

void lay_reserve_items_capacity(lay_context *ctx, lay_id count)
{
    if (count >= ctx->capacity)
    {
        ctx->capacity = count;
        const size_t item_size = sizeof(lay_item_t) + sizeof(lay_vec4);
        ctx->items = (lay_item_t *)LAY_REALLOC(ctx->items, ctx->capacity * item_size);
        const lay_item_t *past_last = ctx->items + ctx->capacity;
        ctx->rects = (lay_vec4 *)past_last;
    }
}

void lay_destroy_context(lay_context *ctx)
{
    if (ctx->items != NULL)
    {
        LAY_FREE(ctx->items);
        ctx->items = NULL;
        ctx->rects = NULL;
    }
}

void lay_reset_context(lay_context *ctx)
{
    ctx->count = 0;
}

static void lay_calc_size(lay_context *ctx, lay_id item, int dim);
static void lay_arrange(lay_context *ctx, lay_id item, int dim);

void lay_run_context(lay_context *ctx)
{
    LAY_ASSERT(ctx != NULL);

    if (ctx->count > 0)
    {
        lay_run_item(ctx, 0);
    }
}

void lay_run_item(lay_context *ctx, lay_id item)
{
    LAY_ASSERT(ctx != NULL);

    lay_calc_size(ctx, item, 0);
    lay_arrange(ctx, item, 0);
    lay_calc_size(ctx, item, 1);
    lay_arrange(ctx, item, 1);
}

// Alternatively, we could use a flag bit to indicate whether an item's children
// have already been wrapped and may need re-wrapping. If we do that, in the
// future, this would become deprecated and we could make it a no-op.

void lay_clear_item_break(lay_context *ctx, lay_id item)
{
    LAY_ASSERT(ctx != NULL);
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->flags = pitem->flags & ~LAY_BREAK;
}

lay_id lay_items_count(lay_context *ctx)
{
    LAY_ASSERT(ctx != NULL);
    return ctx->count;
}

lay_id lay_items_capacity(lay_context *ctx)
{
    LAY_ASSERT(ctx != NULL);
    return ctx->capacity;
}

lay_id lay_item(lay_context *ctx)
{
    lay_id idx = ctx->count++;

    if (idx >= ctx->capacity)
    {
        ctx->capacity = ctx->capacity < 1 ? 32 : (ctx->capacity * 4);
        const size_t item_size = sizeof(lay_item_t) + sizeof(lay_vec4);
        ctx->items = (lay_item_t *)LAY_REALLOC(ctx->items, ctx->capacity * item_size);
        const lay_item_t *past_last = ctx->items + ctx->capacity;
        ctx->rects = (lay_vec4 *)past_last;
    }

    lay_item_t *item = lay_get_item(ctx, idx);
    // We can either do this here, or when creating/resetting buffer
    LAY_MEMSET(item, 0, sizeof(lay_item_t));
    item->first_child = LAY_INVALID_ID;
    item->next_sibling = LAY_INVALID_ID;
    // hmm
    LAY_MEMSET(&ctx->rects[idx], 0, sizeof(lay_vec4));
    return idx;
}

static LAY_FORCE_INLINE void lay_append_by_ptr(
    lay_item_t *LAY_RESTRICT pearlier,
    lay_id later, lay_item_t *LAY_RESTRICT plater)
{
    plater->next_sibling = pearlier->next_sibling;
    plater->flags |= LAY_ITEM_INSERTED;
    pearlier->next_sibling = later;
}

lay_id lay_last_child(const lay_context *ctx, lay_id parent)
{
    lay_item_t *pparent = lay_get_item(ctx, parent);
    lay_id child = pparent->first_child;
    if (child == LAY_INVALID_ID)
        return LAY_INVALID_ID;
    lay_item_t *pchild = lay_get_item(ctx, child);
    lay_id result = child;
    for (;;)
    {
        lay_id next = pchild->next_sibling;
        if (next == LAY_INVALID_ID)
            break;
        result = next;
        pchild = lay_get_item(ctx, next);
    }
    return result;
}

void lay_append(lay_context *ctx, lay_id earlier, lay_id later)
{
    LAY_ASSERT(later != 0);       // Must not be root item
    LAY_ASSERT(earlier != later); // Must not be same item id
    lay_item_t *LAY_RESTRICT pearlier = lay_get_item(ctx, earlier);
    lay_item_t *LAY_RESTRICT plater = lay_get_item(ctx, later);
    lay_append_by_ptr(pearlier, later, plater);
}

void lay_insert(lay_context *ctx, lay_id parent, lay_id child)
{
    LAY_ASSERT(child != 0);      // Must not be root item
    LAY_ASSERT(parent != child); // Must not be same item id
    lay_item_t *LAY_RESTRICT pparent = lay_get_item(ctx, parent);
    lay_item_t *LAY_RESTRICT pchild = lay_get_item(ctx, child);
    LAY_ASSERT(!(pchild->flags & LAY_ITEM_INSERTED));
    // Parent has no existing children, make inserted item the first child.
    if (pparent->first_child == LAY_INVALID_ID)
    {
        pparent->first_child = child;
        pchild->flags |= LAY_ITEM_INSERTED;
        // Parent has existing items, iterate to find the last child and append the
        // inserted item after it.
    }
    else
    {
        lay_id next = pparent->first_child;
        lay_item_t *LAY_RESTRICT pnext = lay_get_item(ctx, next);
        for (;;)
        {
            next = pnext->next_sibling;
            if (next == LAY_INVALID_ID)
                break;
            pnext = lay_get_item(ctx, next);
        }
        lay_append_by_ptr(pnext, child, pchild);
    }
}

void lay_push(lay_context *ctx, lay_id parent, lay_id new_child)
{
    LAY_ASSERT(new_child != 0);      // Must not be root item
    LAY_ASSERT(parent != new_child); // Must not be same item id
    lay_item_t *LAY_RESTRICT pparent = lay_get_item(ctx, parent);
    lay_id old_child = pparent->first_child;
    lay_item_t *LAY_RESTRICT pchild = lay_get_item(ctx, new_child);
    LAY_ASSERT(!(pchild->flags & LAY_ITEM_INSERTED));
    pparent->first_child = new_child;
    pchild->flags |= LAY_ITEM_INSERTED;
    pchild->next_sibling = old_child;
}

lay_vec2 lay_get_size(lay_context *ctx, lay_id item)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    return pitem->size;
}

void lay_get_size_xy(
    lay_context *ctx, lay_id item,
    lay_scalar *x, lay_scalar *y)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    lay_vec2 size = pitem->size;
    *x = size[0];
    *y = size[1];
}

void lay_set_size(lay_context *ctx, lay_id item, lay_vec2 size)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->size = size;
    uint32_t flags = pitem->flags;
    if (size[0] == 0)
        flags &= ~LAY_ITEM_HFIXED;
    else
        flags |= LAY_ITEM_HFIXED;
    if (size[1] == 0)
        flags &= ~LAY_ITEM_VFIXED;
    else
        flags |= LAY_ITEM_VFIXED;
    pitem->flags = flags;
}

void lay_set_size_xy(
    lay_context *ctx, lay_id item,
    lay_scalar width, lay_scalar height)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->size[0] = width;
    pitem->size[1] = height;
    // Kinda redundant, whatever
    uint32_t flags = pitem->flags;
    if (width == 0)
        flags &= ~LAY_ITEM_HFIXED;
    else
        flags |= LAY_ITEM_HFIXED;
    if (height == 0)
        flags &= ~LAY_ITEM_VFIXED;
    else
        flags |= LAY_ITEM_VFIXED;
    pitem->flags = flags;
}

void lay_set_behave(lay_context *ctx, lay_id item, uint32_t flags)
{
    LAY_ASSERT((flags & LAY_ITEM_LAYOUT_MASK) == flags);
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->flags = (pitem->flags & ~LAY_ITEM_LAYOUT_MASK) | flags;
}

void lay_set_contain(lay_context *ctx, lay_id item, uint32_t flags)
{
    LAY_ASSERT((flags & LAY_ITEM_BOX_MASK) == flags);
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->flags = (pitem->flags & ~LAY_ITEM_BOX_MASK) | flags;
}
void lay_set_margins(lay_context *ctx, lay_id item, lay_vec4 ltrb)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    pitem->margins = ltrb;
}
void lay_set_margins_ltrb(
    lay_context *ctx, lay_id item,
    lay_scalar l, lay_scalar t, lay_scalar r, lay_scalar b)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    // Alternative, uses stack and addressed writes
    //pitem->margins = lay_vec4_xyzw(l, t, r, b);
    // Alternative, uses rax and left-shift
    //pitem->margins = (lay_vec4){l, t, r, b};
    // Fewest instructions, but uses more addressed writes?
    pitem->margins[0] = l;
    pitem->margins[1] = t;
    pitem->margins[2] = r;
    pitem->margins[3] = b;
}

lay_vec4 lay_get_margins(lay_context *ctx, lay_id item)
{
    return lay_get_item(ctx, item)->margins;
}

void lay_get_margins_ltrb(
    lay_context *ctx, lay_id item,
    lay_scalar *l, lay_scalar *t, lay_scalar *r, lay_scalar *b)
{
    lay_item_t *pitem = lay_get_item(ctx, item);
    lay_vec4 margins = pitem->margins;
    *l = margins[0];
    *t = margins[1];
    *r = margins[2];
    *b = margins[3];
}

// TODO restrict item ptrs correctly
static LAY_FORCE_INLINE
    lay_scalar
    lay_calc_overlayed_size(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *LAY_RESTRICT pitem = lay_get_item(ctx, item);
    lay_scalar need_size = 0;
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID)
    {
        lay_item_t *pchild = lay_get_item(ctx, child);
        lay_vec4 rect = ctx->rects[child];
        // width = start margin + calculated width + end margin
        lay_scalar child_size = rect[dim] + rect[2 + dim] + pchild->margins[wdim];
        need_size = lay_scalar_max(need_size, child_size);
        child = pchild->next_sibling;
    }
    return need_size;
}

static LAY_FORCE_INLINE
    lay_scalar
    lay_calc_stacked_size(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *LAY_RESTRICT pitem = lay_get_item(ctx, item);
    lay_scalar need_size = 0;
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID)
    {
        lay_item_t *pchild = lay_get_item(ctx, child);
        lay_vec4 rect = ctx->rects[child];
        need_size += rect[dim] + rect[2 + dim] + pchild->margins[wdim];
        child = pchild->next_sibling;
    }
    return need_size;
}

static LAY_FORCE_INLINE
    lay_scalar
    lay_calc_wrapped_overlayed_size(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *LAY_RESTRICT pitem = lay_get_item(ctx, item);
    lay_scalar need_size = 0;
    lay_scalar need_size2 = 0;
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID)
    {
        lay_item_t *pchild = lay_get_item(ctx, child);
        lay_vec4 rect = ctx->rects[child];
        if (pchild->flags & LAY_BREAK)
        {
            need_size2 += need_size;
            need_size = 0;
        }
        lay_scalar child_size = rect[dim] + rect[2 + dim] + pchild->margins[wdim];
        need_size = lay_scalar_max(need_size, child_size);
        child = pchild->next_sibling;
    }
    return need_size2 + need_size;
}

// Equivalent to uiComputeWrappedStackedSize
static LAY_FORCE_INLINE
    lay_scalar
    lay_calc_wrapped_stacked_size(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *LAY_RESTRICT pitem = lay_get_item(ctx, item);
    lay_scalar need_size = 0;
    lay_scalar need_size2 = 0;
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID)
    {
        lay_item_t *pchild = lay_get_item(ctx, child);
        lay_vec4 rect = ctx->rects[child];
        if (pchild->flags & LAY_BREAK)
        {
            need_size2 = lay_scalar_max(need_size2, need_size);
            need_size = 0;
        }
        need_size += rect[dim] + rect[2 + dim] + pchild->margins[wdim];
        child = pchild->next_sibling;
    }
    return lay_scalar_max(need_size2, need_size);
}

static void lay_calc_size(lay_context *ctx, lay_id item, int dim)
{
    lay_item_t *pitem = lay_get_item(ctx, item);

    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID)
    {
        // NOTE: this is recursive and will run out of stack space if items are
        // nested too deeply.
        lay_calc_size(ctx, child, dim);
        lay_item_t *pchild = lay_get_item(ctx, child);
        child = pchild->next_sibling;
    }

    // Set the mutable rect output data to the starting input data
    ctx->rects[item][dim] = pitem->margins[dim];

    // If we have an explicit input size, just set our output size (which other
    // calc_size and arrange procedures will use) to it.
    if (pitem->size[dim] != 0)
    {
        ctx->rects[item][2 + dim] = pitem->size[dim];
        return;
    }

    // Calculate our size based on children items. Note that we've already
    // called lay_calc_size on our children at this point.
    lay_scalar cal_size;
    switch (pitem->flags & LAY_ITEM_BOX_MODEL_MASK)
    {
    case LAY_COLUMN | LAY_WRAP:
        // flex model
        if (dim) // direction
            cal_size = lay_calc_stacked_size(ctx, item, 1);
        else
            cal_size = lay_calc_overlayed_size(ctx, item, 0);
        break;
    case LAY_ROW | LAY_WRAP:
        // flex model
        if (!dim) // direction
            cal_size = lay_calc_wrapped_stacked_size(ctx, item, 0);
        else
            cal_size = lay_calc_wrapped_overlayed_size(ctx, item, 1);
        break;
    case LAY_COLUMN:
    case LAY_ROW:
        // flex model
        if ((pitem->flags & 1) == (uint32_t)dim) // direction
            cal_size = lay_calc_stacked_size(ctx, item, dim);
        else
            cal_size = lay_calc_overlayed_size(ctx, item, dim);
        break;
    default:
        // layout model
        cal_size = lay_calc_overlayed_size(ctx, item, dim);
        break;
    }

    // Set our output data size. Will be used by parent calc_size procedures.,
    // and by arrange procedures.
    ctx->rects[item][2 + dim] = cal_size;
}

static LAY_FORCE_INLINE void lay_arrange_stacked(
    lay_context *ctx, lay_id item, int dim, bool wrap)
{
    const int wdim = dim + 2;
    lay_item_t *pitem = lay_get_item(ctx, item);

    const uint32_t item_flags = pitem->flags;
    lay_vec4 rect = ctx->rects[item];
    lay_scalar space = rect[2 + dim];

    float max_x2 = (float)(rect[dim] + space);

    lay_id start_child = pitem->first_child;
    while (start_child != LAY_INVALID_ID)
    {
        lay_scalar used = 0;
        uint32_t count = 0;          // count of fillers
        uint32_t squeezed_count = 0; // count of squeezable elements
        uint32_t total = 0;
        bool hardbreak = false;
        // first pass: count items that need to be expanded,
        // and the space that is used
        lay_id child = start_child;
        lay_id end_child = LAY_INVALID_ID;
        while (child != LAY_INVALID_ID)
        {
            lay_item_t *pchild = lay_get_item(ctx, child);
            const uint32_t child_flags = pchild->flags;
            const uint32_t flags = (child_flags & LAY_ITEM_LAYOUT_MASK) >> dim;
            const uint32_t fflags = (child_flags & LAY_ITEM_FIXED_MASK) >> dim;
            const lay_vec4 child_margins = pchild->margins;
            lay_vec4 child_rect = ctx->rects[child];
            lay_scalar extend = used;
            if ((flags & LAY_HFILL) == LAY_HFILL)
            {
                ++count;
                extend += child_rect[dim] + child_margins[wdim];
            }
            else
            {
                if ((fflags & LAY_ITEM_HFIXED) != LAY_ITEM_HFIXED)
                    ++squeezed_count;
                extend += child_rect[dim] + child_rect[2 + dim] + child_margins[wdim];
            }
            // wrap on end of line or manual flag
            if (wrap && (total && ((extend > space) ||
                                   (child_flags & LAY_BREAK))))
            {
                end_child = child;
                hardbreak = (child_flags & LAY_BREAK) == LAY_BREAK;
                // add marker for subsequent queries
                pchild->flags = child_flags | LAY_BREAK;
                break;
            }
            else
            {
                used = extend;
                child = pchild->next_sibling;
            }
            ++total;
        }

        lay_scalar extra_space = space - used;
        float filler = 0.0f;
        float spacer = 0.0f;
        float extra_margin = 0.0f;
        float eater = 0.0f;

        if (extra_space > 0)
        {
            if (count > 0)
                filler = (float)extra_space / (float)count;
            else if (total > 0)
            {
                switch (item_flags & LAY_JUSTIFY)
                {
                case LAY_JUSTIFY:
                    // justify when not wrapping or not in last line,
                    // or not manually breaking
                    if (!wrap || ((end_child != LAY_INVALID_ID) && !hardbreak))
                        spacer = (float)extra_space / (float)(total - 1);
                    break;
                case LAY_START:
                    break;
                case LAY_END:
                    extra_margin = extra_space;
                    break;
                default:
                    extra_margin = extra_space / 2.0f;
                    break;
                }
            }
        }

#ifdef LAY_FLOAT
        // In floating point, it's possible to end up with some small negative
        // value for extra_space, while also have a 0.0 squeezed_count. This
        // would cause divide by zero. Instead, we'll check to see if
        // squeezed_count is > 0. I believe this produces the same results as
        // the original oui int-only code. However, I don't have any tests for
        // it, so I'll leave it if-def'd for now.
        else if (!wrap && (squeezed_count > 0))
#else
        // This is the original oui code
        else if (!wrap && (extra_space < 0))
#endif
            eater = (float)extra_space / (float)squeezed_count;

        // distribute width among items
        float x = (float)rect[dim];
        float x1;
        // second pass: distribute and rescale
        child = start_child;
        while (child != end_child)
        {
            lay_scalar ix0, ix1;
            lay_item_t *pchild = lay_get_item(ctx, child);
            const uint32_t child_flags = pchild->flags;
            const uint32_t flags = (child_flags & LAY_ITEM_LAYOUT_MASK) >> dim;
            const uint32_t fflags = (child_flags & LAY_ITEM_FIXED_MASK) >> dim;
            const lay_vec4 child_margins = pchild->margins;
            lay_vec4 child_rect = ctx->rects[child];

            x += (float)child_rect[dim] + extra_margin;
            if ((flags & LAY_HFILL) == LAY_HFILL) // grow
                x1 = x + filler;
            else if ((fflags & LAY_ITEM_HFIXED) == LAY_ITEM_HFIXED)
                x1 = x + (float)child_rect[2 + dim];
            else // squeeze
                x1 = x + lay_float_max(0.0f, (float)child_rect[2 + dim] + eater);

            ix0 = (lay_scalar)x;
            if (wrap)
                ix1 = (lay_scalar)lay_float_min(max_x2 - (float)child_margins[wdim], x1);
            else
                ix1 = (lay_scalar)x1;
            child_rect[dim] = ix0;           // pos
            child_rect[dim + 2] = ix1 - ix0; // size
            ctx->rects[child] = child_rect;
            x = x1 + (float)child_margins[wdim];
            child = pchild->next_sibling;
            extra_margin = spacer;
        }

        start_child = end_child;
    }
}

static LAY_FORCE_INLINE void lay_arrange_overlay(lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *pitem = lay_get_item(ctx, item);
    const lay_vec4 rect = ctx->rects[item];
    const lay_scalar offset = rect[dim];
    const lay_scalar space = rect[2 + dim];

    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID)
    {
        lay_item_t *pchild = lay_get_item(ctx, child);
        const uint32_t b_flags = (pchild->flags & LAY_ITEM_LAYOUT_MASK) >> dim;
        const lay_vec4 child_margins = pchild->margins;
        lay_vec4 child_rect = ctx->rects[child];

        switch (b_flags & LAY_HFILL)
        {
        case LAY_HCENTER:
            child_rect[dim] += (space - child_rect[2 + dim]) / 2 - child_margins[wdim];
            break;
        case LAY_RIGHT:
            child_rect[dim] += space - child_rect[2 + dim] - child_margins[wdim];
            break;
        case LAY_HFILL:
            child_rect[2 + dim] = lay_scalar_max(0, space - child_rect[dim] - child_margins[wdim]);
            break;
        default:
            break;
        }

        child_rect[dim] += offset;
        ctx->rects[child] = child_rect;
        child = pchild->next_sibling;
    }
}

static LAY_FORCE_INLINE void lay_arrange_overlay_squeezed_range(
    lay_context *ctx, int dim,
    lay_id start_item, lay_id end_item,
    lay_scalar offset, lay_scalar space)
{
    int wdim = dim + 2;
    lay_id item = start_item;
    while (item != end_item)
    {
        lay_item_t *pitem = lay_get_item(ctx, item);
        const uint32_t b_flags = (pitem->flags & LAY_ITEM_LAYOUT_MASK) >> dim;
        const lay_vec4 margins = pitem->margins;
        lay_vec4 rect = ctx->rects[item];
        lay_scalar min_size = lay_scalar_max(0, space - rect[dim] - margins[wdim]);
        switch (b_flags & LAY_HFILL)
        {
        case LAY_HCENTER:
            rect[2 + dim] = lay_scalar_min(rect[2 + dim], min_size);
            rect[dim] += (space - rect[2 + dim]) / 2 - margins[wdim];
            break;
        case LAY_RIGHT:
            rect[2 + dim] = lay_scalar_min(rect[2 + dim], min_size);
            rect[dim] = space - rect[2 + dim] - margins[wdim];
            break;
        case LAY_HFILL:
            rect[2 + dim] = min_size;
            break;
        default:
            rect[2 + dim] = lay_scalar_min(rect[2 + dim], min_size);
            break;
        }
        rect[dim] += offset;
        ctx->rects[item] = rect;
        item = pitem->next_sibling;
    }
}

static LAY_FORCE_INLINE
    lay_scalar
    lay_arrange_wrapped_overlay_squeezed(
        lay_context *ctx, lay_id item, int dim)
{
    const int wdim = dim + 2;
    lay_item_t *pitem = lay_get_item(ctx, item);
    lay_scalar offset = ctx->rects[item][dim];
    lay_scalar need_size = 0;
    lay_id child = pitem->first_child;
    lay_id start_child = child;
    while (child != LAY_INVALID_ID)
    {
        lay_item_t *pchild = lay_get_item(ctx, child);
        if (pchild->flags & LAY_BREAK)
        {
            lay_arrange_overlay_squeezed_range(ctx, dim, start_child, child, offset, need_size);
            offset += need_size;
            start_child = child;
            need_size = 0;
        }
        const lay_vec4 rect = ctx->rects[child];
        lay_scalar child_size = rect[dim] + rect[2 + dim] + pchild->margins[wdim];
        need_size = lay_scalar_max(need_size, child_size);
        child = pchild->next_sibling;
    }
    lay_arrange_overlay_squeezed_range(ctx, dim, start_child, LAY_INVALID_ID, offset, need_size);
    offset += need_size;
    return offset;
}

static void lay_arrange(lay_context *ctx, lay_id item, int dim)
{
    lay_item_t *pitem = lay_get_item(ctx, item);

    const uint32_t flags = pitem->flags;
    switch (flags & LAY_ITEM_BOX_MODEL_MASK)
    {
    case LAY_COLUMN | LAY_WRAP:
        if (dim != 0)
        {
            lay_arrange_stacked(ctx, item, 1, true);
            lay_scalar offset = lay_arrange_wrapped_overlay_squeezed(ctx, item, 0);
            ctx->rects[item][2 + 0] = offset - ctx->rects[item][0];
        }
        break;
    case LAY_ROW | LAY_WRAP:
        if (dim == 0)
            lay_arrange_stacked(ctx, item, 0, true);
        else
            // discard return value
            lay_arrange_wrapped_overlay_squeezed(ctx, item, 1);
        break;
    case LAY_COLUMN:
    case LAY_ROW:
        if ((flags & 1) == (uint32_t)dim)
        {
            lay_arrange_stacked(ctx, item, dim, false);
        }
        else
        {
            const lay_vec4 rect = ctx->rects[item];
            lay_arrange_overlay_squeezed_range(
                ctx, dim, pitem->first_child, LAY_INVALID_ID,
                rect[dim], rect[2 + dim]);
        }
        break;
    default:
        lay_arrange_overlay(ctx, item, dim);
        break;
    }
    lay_id child = pitem->first_child;
    while (child != LAY_INVALID_ID)
    {
        // NOTE: this is recursive and will run out of stack space if items are
        // nested too deeply.
        lay_arrange(ctx, child, dim);
        lay_item_t *pchild = lay_get_item(ctx, child);
        child = pchild->next_sibling;
    }
}

#endif // LAY_IMPLEMENTATION