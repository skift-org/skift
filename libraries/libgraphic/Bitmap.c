/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Bitmap.h>
#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/memory.h>

#define LODEPNG_NO_COMPILE_DISK
#define LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS

#include <thirdparty/lodepng/lodepng.h>

#include <thirdparty/lodepng/lodepng.cpp>

Bitmap *bitmap_create(size_t width, size_t height)
{
    Bitmap *bitmap = NULL;

    shared_memory_alloc(sizeof(Bitmap) + width * height * sizeof(Color), (uintptr_t *)&bitmap);
    assert(bitmap);

    bitmap->width = width;
    bitmap->height = height;
    bitmap->filtering = BITMAP_FILTERING_NEAREST;

    return bitmap;
}

void bitmap_destroy(Bitmap *bitmap)
{
    shared_memory_free((uintptr_t)bitmap);
}

Rectangle bitmap_bound(Bitmap *bitmap)
{
    return (Rectangle){{0, 0, bitmap->width, bitmap->height}};
}

static Color placeholder_buffer[] = {
    (Color){{255, 0, 255, 255}},
    (Color){{0, 0, 0, 255}},
    (Color){{0, 0, 0, 255}},
    (Color){{255, 0, 255, 255}},
};

Bitmap *bitmap_load_from(const char *path)
{
    uint width = 0;
    uint height = 0;
    void *rawdata = NULL;
    void *outdata = NULL;

    Stream *file = stream_open(path, OPEN_READ);
    Bitmap *bitmap = NULL;

    if (handle_has_error(file))
    {
        logger_error("Failled to load bitmap from %s: %s", path, handle_error_string(file));
        goto cleanup_and_return;
    }

    FileState state;
    stream_stat(file, &state);

    rawdata = malloc(state.size);
    assert(stream_read(file, rawdata, state.size) == state.size);

    int decode_result = lodepng_decode32((unsigned char **)&outdata, &width, &height, (const unsigned char *)rawdata, state.size);

    if (decode_result != 0)
    {
        logger_error("Failled to decode bitmap from %s: %s", path, lodepng_error_text(decode_result));
        goto cleanup_and_return;
    }

    bitmap = bitmap_create(width, height);
    memcpy(bitmap->pixels, outdata, width * height * sizeof(Color));

cleanup_and_return:
    if (rawdata)
    {
        free(rawdata);
    }

    if (outdata)
    {
        free(outdata);
    }

    stream_close(file);

    if (bitmap == NULL)
    {
        bitmap = bitmap_create(2, 2);
        memcpy(bitmap->pixels, placeholder_buffer, 2 * 2 * sizeof(Color));
    }

    return bitmap;
}

Result bitmap_save_to(Bitmap *bitmap, const char *path)
{
    Result result = SUCCESS;
    Stream *file = stream_open(path, OPEN_WRITE);

    if (handle_has_error(file))
    {
        result = handle_get_error(file);
        goto cleanup_and_return;
    }

    void *outbuffer = NULL;
    size_t outbuffer_size = 0;

    int err = lodepng_encode_memory((unsigned char **)&outbuffer, &outbuffer_size, (const unsigned char *)bitmap->pixels, bitmap->width, bitmap->height, LCT_RGBA, 8);

    if (err != 0)
    {
        result = ERR_BAD_IMAGE_FILE_FORMAT;
        goto cleanup_and_return;
    }

    stream_write(file, outbuffer, outbuffer_size);

    if (handle_has_error(file))
    {
        result = handle_get_error(file);
        goto cleanup_and_return;
    }

cleanup_and_return:
    if (file)
    {
        stream_close(file);
    }

    if (outbuffer)
    {
        free(outbuffer);
    }

    return result;
}