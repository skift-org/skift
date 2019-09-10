#include "stream.h"

stream_t *stream(struct fsnode *node, iostream_flag_t flags)
{
    if (node == NULL)
        return NULL;

    stream_t *s = MALLOC(stream_t);

    s->node = node;
    s->offset = 0;
    s->flags = flags;

    return s;
}

void stream_delete(stream_t *s)
{
    free(s);
}