#include <libgraphic/font/TrueTypeFontFace.h>
#include <libio/File.h>
#include <libutils/Assert.h>

int main(int argc, const char *argv[])
{
    if(argc < 2)
    {
      return EXIT_FAILURE;
    }
    IO::File file{argv[1], OPEN_READ};
    auto font = Graphic::SuperCoolFont::TrueTypeFontFace::load(file);

    Assert::is_true(font.success());

    return EXIT_SUCCESS;
}