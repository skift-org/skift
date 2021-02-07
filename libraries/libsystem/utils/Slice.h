#pragma once

#include <assert.h>

struct Slice
{
    unsigned char *data;
    int cursor;
    int size;
};

#define SLICE_NULL ((Slice){nullptr, 0, 0})

static inline Slice slice_create(const void *p, size_t size)
{
    return (Slice){
        .data = (uint8_t *)p,
        .cursor = 0,
        .size = (int)size,
    };
}

static inline Slice slice_range(const Slice *slice, int offset, int size)
{
    if (offset < 0 || size < 0 || offset > slice->size || size > slice->size - offset)
    {
        return SLICE_NULL;
    }
    else
    {
        return slice_create(slice->data + offset, size);
    }
}

static inline Slice slice_sub_slice(Slice slice, int offset, int size)
{
    return slice_create(slice.data + offset, size);
}

static inline bool slice_is_empty(Slice slice)
{
    return slice.size == 0;
}

#define slice_get32(b) slice_get((b), 4)
#define slice_get16(b) slice_get((b), 2)
static inline uint8_t slice_get8(Slice *slice)
{
    if (slice->cursor >= slice->size)
        return 0;

    return slice->data[slice->cursor++];
}

static inline uint32_t slice_get(Slice *slice, int n)
{
    assert(n >= 1 && n <= 4);

    uint32_t value = 0;

    for (int i = 0; i < n; i++)
    {
        value = (value << 8) | slice_get8(slice);
    }

    return value;
}

static inline uint8_t slice_peek8(Slice *slice)
{
    if (slice->cursor >= slice->size)
        return 0;

    return slice->data[slice->cursor];
}

static inline void slice_seek(Slice *slice, int offset)
{
    assert(!(offset > slice->size || offset < 0));
    slice->cursor = (offset > slice->size || offset < 0) ? slice->size : offset;
}

static inline void slice_skip(Slice *slice, int offset)
{
    slice_seek(slice, slice->cursor + offset);
}
