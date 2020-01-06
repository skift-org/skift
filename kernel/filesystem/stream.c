#include "filesystem/stream.h"

stream_t *stream(struct fsnode *node, IOStreamFlag flags)
{
    if (node == NULL)
        return NULL;

    stream_t *s = __malloc(stream_t);

    s->node = node;
    s->offset = 0;
    s->flags = flags;

    return s;
}

void stream_delete(stream_t *s)
{
    free(s);
}