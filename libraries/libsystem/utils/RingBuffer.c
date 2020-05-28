
#include <libsystem/Assert.h>
#include <libsystem/utils/RingBuffer.h>

RingBuffer *ringbuffer_create(size_t size)
{
    RingBuffer *ringbuffer = (RingBuffer *)calloc(1, sizeof(RingBuffer) + size);

    ringbuffer->size = size;

    return ringbuffer;
}

void ringbuffer_destroy(RingBuffer *ringbuffer)
{
    free(ringbuffer);
}

bool ringbuffer_is_empty(RingBuffer *ringbuffer)
{
    return !ringbuffer_used(ringbuffer);
}

bool ringbuffer_is_full(RingBuffer *ringbuffer)
{
    return ringbuffer_used(ringbuffer) == ringbuffer->size;
}

size_t ringbuffer_used(RingBuffer *ringbuffer)
{
    return ringbuffer->used;
}

void ringbuffer_putc(RingBuffer *ringbuffer, char c)
{
    assert(!ringbuffer_is_full(ringbuffer));

    ringbuffer->buffer[ringbuffer->head] = c;
    ringbuffer->head = (ringbuffer->head + 1) % (ringbuffer->size);
    ringbuffer->used++;
}

char ringbuffer_getc(RingBuffer *ringbuffer)
{
    assert(!ringbuffer_is_empty(ringbuffer));

    char c = ringbuffer->buffer[ringbuffer->tail];
    ringbuffer->tail = (ringbuffer->tail + 1) % (ringbuffer->size);
    ringbuffer->used--;

    return c;
}

char ringbuffer_peek(RingBuffer *ringbuffer, size_t peek)
{
    int offset = (ringbuffer->tail + peek) % (ringbuffer->size);

    return ringbuffer->buffer[offset];
}

size_t ringbuffer_read(RingBuffer *ringbuffer, char *buffer, size_t size)
{
    size_t read = 0;

    while (!ringbuffer_is_empty(ringbuffer) && read < size)
    {
        buffer[read] = ringbuffer_getc(ringbuffer);
        read++;
    }

    return read;
}

size_t ringbuffer_write(RingBuffer *ringbuffer, const char *buffer, size_t size)
{
    size_t written = 0;

    while (!ringbuffer_is_full(ringbuffer) && written < size)
    {
        ringbuffer_putc(ringbuffer, buffer[written]);
        written++;
    }

    return written;
}
