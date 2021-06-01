#include <libio/Directory.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libxml/Parser.h>

#include "tests/Driver.h"

TEST(xml_all_svg_test)
{
    IO::Directory icon_dir{"/Files/Icons"};

    Assert::truth(icon_dir.exist());

    for (const auto &entry : icon_dir.entries())
    {
        auto path = IO::Path::parse(IO::format("/Files/Icons/{}", entry.name));
        if (path.extension() != ".svg")
        {
            continue;
        }

        IO::logln("Parsing {}", path);

        IO::File file(path, HJ_OPEN_READ);
        Assert::truth(file.exist());

        auto result = Xml::parse(file);
        Assert::equal(result.result(), HjResult::SUCCESS);
    }
}