#define LODEPNG_NO_COMPILE_DISK
#define LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS
#define LODEPNG_NO_COMPILE_CPP
#include <thirdparty/lodepng/lodepng.cpp>
#undef LODEPNG_NO_COMPILE_CPP
#undef LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS
#undef LODEPNG_NO_COMPILE_DISK

#include <libgraphic/Bitmap.h>
#include <libsystem/Assert.h>
#include <libsystem/Result.h>
#include <libsystem/io/File.h>
#include <libsystem/Logger.h>
#include <libsystem/system/Memory.h>

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

Result bitmap_load_from_can_fail(const char *path, Bitmap **bitmap)
{
    void *rawdata;
    size_t rawdata_size;
    Result result = file_read_all(path, &rawdata, &rawdata_size);

    if (result != SUCCESS)
    {
        return result;
    }

    uint decoded_width = 0;
    uint decoded_height = 0;
    void *decoded_data __cleanup_malloc = NULL;

    int decode_result = lodepng_decode32(
        (unsigned char **)&decoded_data,
        &decoded_width,
        &decoded_height,
        (const unsigned char *)rawdata,
        rawdata_size);

    if (decode_result != 0)
    {
        return ERR_BAD_IMAGE_FILE_FORMAT;
    }

    size_t decoded_size = decoded_width * decoded_height * sizeof(Color);

    *bitmap = bitmap_create(decoded_width, decoded_height);

    memcpy((*bitmap)->pixels, decoded_data, decoded_size);

    return SUCCESS;
}

Bitmap *bitmap_load_from(const char *path)
{
    Bitmap *bitmap = NULL;
    Result result = bitmap_load_from_can_fail(path, &bitmap);

    if (result != SUCCESS)
    {
        bitmap = bitmap_create(2, 2);
        memcpy(bitmap->pixels, placeholder_buffer, 2 * 2 * sizeof(Color));
    }

    return bitmap;
}

Result bitmap_save_to(Bitmap *bitmap, const char *path)
{
    void *outbuffer __cleanup_malloc = NULL;

    size_t outbuffer_size = 0;

    int err = lodepng_encode_memory((unsigned char **)&outbuffer, &outbuffer_size, (const unsigned char *)bitmap->pixels, bitmap->width, bitmap->height, LCT_RGBA, 8);

    if (err != 0)
    {
        return ERR_BAD_IMAGE_FILE_FORMAT;
    }

    return file_write_all(path, outbuffer, outbuffer_size);
}

__flatten void bitmap_copy(Bitmap *source, Bitmap *destination, Rectangle region)
{
    region = rectangle_clip(region, bitmap_bound(source));
    region = rectangle_clip(region, bitmap_bound(destination));

    if (rectangle_is_empty(region))
        return;

    for (int y = region.y; y < region.y + region.height; y++)
    {
        for (int x = region.x; x < region.x + region.width; x++)
        {
            bitmap_set_pixel_no_check(destination, vec2i(x, y), bitmap_get_pixel_no_check(source, vec2i(x, y)));
        }
    }
}
