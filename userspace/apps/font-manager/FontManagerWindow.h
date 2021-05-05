#include <libfilepicker/FilePicker.h>
#include <libgraphic/font/FontFace.h>
#include <libgraphic/font/Parser.h>
#include <libgraphic/rast/Rasterizer.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

using namespace Widget;

struct FontManagerWindow : public Window
{
private:
    RefPtr<Graphic::SuperCoolFont::FontFace> _font_face;
    RefPtr<Graphic::Bitmap> _bitmap;
    Codepoint _current_codepoint = 'A';

public:
    FontManagerWindow(String path) : Window(WINDOW_RESIZABLE)
    {
        size(Math::Vec2i(700, 500));
        _bitmap = Graphic::Bitmap::create_shared(200, 200).unwrap();
        load_font(path);
    }

    Result load_font(String path)
    {
        if (!path.null_or_empty())
        {
            IO::logln("Opening font from {}", path);
            IO::File file{path, OPEN_READ};
            _font_face = TRY(Graphic::SuperCoolFont::load(file));
            update_glyph();
        }

        return Result::SUCCESS;
    }

    void update_glyph()
    {
        auto glyph = _font_face->glyph(_current_codepoint);
        _bitmap->clear(Graphic::Colors::WHITE);

        if (glyph.present())
        {
            Graphic::Rasterizer rast{_bitmap};
            rast.fill(glyph.unwrap().edges, Math::Mat3x2f::scale(20.0f), Graphic::Fill{Graphic::Colors::WHITE});
        }
        should_rebuild();
    }

    RefPtr<Element> build() override
    {
        // clang-format off

        return vflow({
            titlebar(Graphic::Icon::get("font-format"), "Font Manager"),
            fill(
                image(_bitmap))
                });

        // clang-format on
    }
};
