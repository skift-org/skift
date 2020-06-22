#pragma once

#include <libsystem/Assert.h>

typedef struct
{
    unsigned char *data;
    int cursor;
    int size;
} Slice;

static inline Slice slice_create(const void *p, size_t size)
{
    return (Slice){
        .data = (uint8_t *)p,
        .size = (int)size,
        .cursor = 0,
    };
}

static inline Slice slice_range(const Slice *slice, int offset, int size)
{
    if (offset < 0 || size < 0 || offset > slice->size || size > slice->size - offset)
    {
        return slice_create(NULL, 0);
    }
    else
    {
        return slice_create(slice->data + offset, size);
    }
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
