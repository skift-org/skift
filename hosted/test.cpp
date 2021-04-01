#include <stdio.h>
#include <stdlib.h>

#include <libgraphic/png/PngReader.h>
#include <libutils/Assert.h>

#include "terminal.png.h"

void assert_failed(const char *expr, const char *file, const char *function, int line)
{
    printf("Assert failled: %s %s %s %d", expr, file, function, line);
    abort();
}

int main(int argc, const char *argv[])
{
    IO::MemoryReader mem_reader(apps_terminal_terminal_png, apps_terminal_terminal_png_size);
    Graphic::PngReader png_reader(mem_reader);
    Assert::is_true(png_reader.valid());
    Assert::equal(png_reader.width(), 32);
    Assert::equal(png_reader.height(), 32);

    return 0;
}