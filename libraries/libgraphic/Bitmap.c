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

void bitmap_set_pixel(Bitmap *bitmap, Point p, Color color)
{
    if ((p.X >= 0 && p.X < bitmap->width) && (p.Y >= 0 && p.Y < bitmap->height))
        bitmap->pixels[(int)(p.X + p.Y * bitmap->width)] = color;
}

Color bitmap_get_pixel(Bitmap *bitmap, Point p)
{
    int xi = abs((int)p.X % bitmap->width);
    int yi = abs((int)p.Y % bitmap->height);

    return bitmap->pixels[xi + yi * bitmap->width];
}

Color bitmap_sample(Bitmap *bitmap, Rectangle src_rect, float x, float y)
{
    Color result;

    float xx = src_rect.width * x;
    float yy = src_rect.height * y;

    int xxi = (int)xx;
    int yyi = (int)yy;

    if (bitmap->filtering == BITMAP_FILTERING_NEAREST)
    {
        result = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi, src_rect.Y + yyi});
    }
    else
    {
        Color c00 = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi, src_rect.Y + yyi});
        Color c10 = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi + 1, src_rect.Y + yyi});
        Color c01 = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi, src_rect.Y + yyi + 1});
        Color c11 = bitmap_get_pixel(bitmap, (Point){src_rect.X + xxi + 1, src_rect.Y + yyi + 1});

        result = color_blerp(c00, c10, c01, c11, xx - xxi, yy - yyi);
    }

    return result;
}

Rectangle bitmap_bound(Bitmap *bitmap)
{
    return (Rectangle){{0, 0, bitmap->width, bitmap->height}};
}

void bitmap_blend_pixel(Bitmap *bitmap, Point p, Color color)
{
    Color bg = bitmap_get_pixel(bitmap, p);
    bitmap_set_pixel(bitmap, p, color_blend(color, bg));
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

    int decode_result = lodepng_decode32((unsigned char **)&outdata, &width, &height, rawdata, state.size);

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

    int err = lodepng_encode_memory((unsigned char **)&outbuffer, &outbuffer_size, (void *)bitmap->pixels, bitmap->width, bitmap->height, LCT_RGBA, 8);

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