#include <libsystem/assert.h>

#include "Shell/Source.h"

Source *source_create_from_stream(Stream *stream)
{
    StreamSource *source = __create(StreamSource);

    source->stream = stream;

    return (Source *)source;
}

Source *source_create_from_string(const char *buffer, size_t size)
{
    StringSource *source = __create(StringSource);

    source->buffer = buffer;
    source->offset = 0;
    source->size = size;

    return (Source *)source;
}

void source_destroy(Source *source)
{
    assert(source->destroy);

    source->destroy(source);

    free(source);
}