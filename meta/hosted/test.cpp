#include <libgraphic/png/PngReader.h>
#include <libio/File.h>
#include <libutils/Assert.h>

int main(int argc, const char *argv[])
{
    IO::File file{"sysroot/Files/Wallpapers/peaks.png", OPEN_READ};
    Graphic::PngReader png_reader(file);

    Assert::truth(png_reader.valid());

    Assert::equal(png_reader.width(), 32);
    Assert::equal(png_reader.height(), 32);

    Assert::unreachable();

    return 0;
}