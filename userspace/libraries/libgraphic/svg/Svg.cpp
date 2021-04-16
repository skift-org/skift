#include <libgraphic/svg/Rasterizer.h>
#include <libgraphic/svg/Svg.h>
#include <libio/NumberScanner.h>
#include <libio/Streams.h>
#include <libxml/Parser.h>

ResultOr<RefPtr<Graphic::Bitmap>> Graphic::Svg::render(IO::Reader &reader, int resolution)
{
    auto doc = TRY(Xml::parse(reader));

    if (doc.root().name() != "svg")
    {
        logger_error("Svg file must begin with svg root element");
        return Result::ERR_INVALID_DATA;
    }
    auto dec_scanner = IO::NumberScanner::decimal();
    int width, height;

    // Width attribute
    {
        auto width_reader = IO::MemoryReader(doc.root().attributes()["width"]);
        auto width_scanner = IO::Scanner(width_reader);
        width = dec_scanner.scan_int(width_scanner).unwrap();
    }
    // Height attribute
    {
        auto height_reader = IO::MemoryReader(doc.root().attributes()["height"]);
        auto height_scanner = IO::Scanner(height_reader);
        height = dec_scanner.scan_int(height_scanner).unwrap();
    }

    auto bitmap = TRY(Bitmap::create_shared(width * resolution, height * resolution));
    bitmap->clear(Colors::TRANSPARENT);
    bitmap->filtering(BitmapFiltering::NEAREST);
    Rasterizer rast{bitmap};

    for (auto child : doc.root().children())
    {
        if (child.name() == "path")
        {
            Color fillColor = Colors::BLACK;
            if (child.attributes().has_key("fill"))
            {
                fillColor = Color::parse(child.attributes()["fill"]);
            }

            auto path = Graphic::Path::parse(child.attributes()["d"].cstring());
            rast.fill(path, Math::Mat3x2f::scale(resolution), Graphic::Fill{fillColor});
        }
        else
        {
            IO::logln("Unhandled svg node: %s", child.name());
        }
    }

    return bitmap;
}