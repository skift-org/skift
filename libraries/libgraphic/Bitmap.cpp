#define LODEPNG_NO_COMPILE_DISK
#define LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS
#define LODEPNG_NO_COMPILE_CPP
#include <thirdparty/lodepng/lodepng.cpp>
#undef LODEPNG_NO_COMPILE_CPP
#undef LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS
#undef LODEPNG_NO_COMPILE_DISK

#include <libgraphic/Bitmap.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/io/File.h>
#include <libsystem/system/Memory.h>

static Color _placeholder_buffer[] = {
    Colors::MAGENTA,
    Colors::BLACK,
    Colors::BLACK,
    Colors::MAGENTA,
};

ResultOr<RefPtr<Bitmap>> Bitmap::create_shared(int width, int height)
{
    Color *pixels = nullptr;
    Result result = memory_alloc(width * height * sizeof(Color), reinterpret_cast<uintptr_t *>(&pixels));

    if (result != SUCCESS)
        return result;

    int handle = -1;
    memory_get_handle(reinterpret_cast<uintptr_t>(pixels), &handle);

    auto bitmap = make<Bitmap>(handle, BITMAP_SHARED, width, height, pixels);
    bitmap->clear(Colors::BLACK);
    return bitmap;
}

ResultOr<RefPtr<Bitmap>> Bitmap::create_shared_from_handle(int handle, Vec2i width_and_height)
{
    Color *pixels = nullptr;
    size_t size = 0;
    Result result = memory_include(handle, reinterpret_cast<uintptr_t *>(&pixels), &size);

    if (result != SUCCESS)
        return result;

    if (size < width_and_height.x() * width_and_height.y() * sizeof(Color))
    {
        memory_free(reinterpret_cast<uintptr_t>(pixels));
        return ERR_BAD_IMAGE_FILE_FORMAT;
    }

    memory_get_handle(reinterpret_cast<uintptr_t>(pixels), &handle);

    return make<Bitmap>(handle, BITMAP_SHARED, width_and_height.x(), width_and_height.y(), pixels);
}

RefPtr<Bitmap> Bitmap::create_static(int width, int height, Color *pixels)
{
    return make<Bitmap>(-1, BITMAP_STATIC, width, height, pixels);
}

ResultOr<RefPtr<Bitmap>> Bitmap::load_from(const char *path)
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
    void *decoded_data = nullptr;

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

    auto bitmap_or_result = Bitmap::create_shared(decoded_width, decoded_height);

    if (bitmap_or_result.success())
    {
        auto bitmap = bitmap_or_result.take_value();
        memcpy(bitmap->pixels(), decoded_data, sizeof(Color) * decoded_width * decoded_height);
        free(decoded_data);
        return bitmap;
    }
    else
    {
        free(decoded_data);
        return bitmap_or_result;
    }
}

RefPtr<Bitmap> Bitmap::load_from_or_placeholder(const char *path)
{
    auto result = load_from(path);

    if (!result.success())
    {
        return create_static(2, 2, _placeholder_buffer);
    }

    return result.take_value();
}

Result Bitmap::save_to(const char *path)
{
    void *outbuffer __cleanup_malloc = nullptr;

    size_t outbuffer_size = 0;

    int err = lodepng_encode_memory(
        (unsigned char **)&outbuffer,
        &outbuffer_size,
        (const unsigned char *)_pixels,
        _width,
        _height,
        LCT_RGBA, 8);

    if (err != 0)
    {
        return ERR_BAD_IMAGE_FILE_FORMAT;
    }

    return file_write_all(path, outbuffer, outbuffer_size);
}

Bitmap::~Bitmap()
{
    if (_storage == BITMAP_SHARED)
        memory_free(reinterpret_cast<uintptr_t>(_pixels));
    else if (_storage == BITMAP_MALLOC)
        free(_pixels);
}
