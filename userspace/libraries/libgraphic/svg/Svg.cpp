#include <libio/BufReader.h>
#include <libio/NumberScanner.h>
#include <libio/Streams.h>
#include <libxml/Parser.h>

#include <libgraphic/rast/Rasterizer.h>
#include <libgraphic/svg/Svg.h>

namespace Graphic::Svg
{

void render_node(Rasterizer &rast, Xml::Node &node, Math::Mat3x2f transformation, Color fillcolor)
{
    for (auto child : node.children())
    {
        Color current = fillcolor;

        if (child.attributes().has_key("fill"))
        {
            current = Color::parse(child.attributes()["fill"]);
        }

        if (child.name() == "path")
        {
            auto path = Graphic::Path::parse(child.attributes()["d"].cstring());
            rast.fill(path, transformation, Graphic::Fill{current});
        }
        else if (child.name() == "g")
        {
            render_node(rast, child, transformation, current);
        }
        else
        {
            IO::logln("Unhandled svg node: {}", child.name());
        }
    }
}

ResultOr<RefPtr<Graphic::Bitmap>> render(IO::Reader &reader, int size_hint)
{
    IO::BufReader buf{reader, 512};
    auto doc = TRY(Xml::parse(buf));

    if (doc.root().name() != "svg")
    {
        IO::logln("Svg file must begin with svg root element");
        return Result::ERR_INVALID_DATA;
    }

    auto dec_scanner = IO::NumberScanner::decimal();

    int width = 1024;
    int height = 1024;

    if (doc.root().attributes().has_key("width"))
    {
        // Width attribute
        auto width_reader = IO::MemoryReader(doc.root().attributes()["width"]);
        auto width_scanner = IO::Scanner(width_reader);
        width = dec_scanner.scan_int(width_scanner).unwrap();
    }

    if (doc.root().attributes().has_key("height"))
    {
        // Height attribute
        auto height_reader = IO::MemoryReader(doc.root().attributes()["height"]);
        auto height_scanner = IO::Scanner(height_reader);
        height = dec_scanner.scan_int(height_scanner).unwrap();
    }

    float scale = 1;

    if (size_hint > 0)
    {
        scale = (float)size_hint / MAX(width, height);
    }

    auto bitmap = TRY(Bitmap::create_shared(width * scale, height * scale));
    bitmap->clear(Colors::TRANSPARENT);
    bitmap->filtering(BitmapFiltering::NEAREST);
    Rasterizer rast{bitmap};

    render_node(rast, doc.root(), Math::Mat3x2f::scale(scale), Colors::BLACK);

    return bitmap;
}

} // namespace Graphic::Svg