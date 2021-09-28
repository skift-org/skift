#include <libpng/Png.h>
#include <libpng/Reader.h>

namespace Png
{

ResultOr<RefPtr<Graphic::Bitmap>> load(IO::Reader &reader)
{
    Reader png_reader{reader};

    if (!png_reader.valid())
    {
        return ERR_BAD_IMAGE_FILE_FORMAT;
    }

    unsigned int decoded_width = png_reader.width();
    unsigned int decoded_height = png_reader.height();
    const Graphic::Color *decoded_data = png_reader.pixels().raw_storage();

    auto bitmap_or_result = Graphic::Bitmap::create_shared(decoded_width, decoded_height);

    if (bitmap_or_result.success())
    {
        auto bitmap = bitmap_or_result.unwrap();
        memcpy(bitmap->pixels(), decoded_data, sizeof(Graphic::Color) * decoded_width * decoded_height);
        return bitmap;
    }
    else
    {
        return bitmap_or_result;
    }
}

} // namespace Png