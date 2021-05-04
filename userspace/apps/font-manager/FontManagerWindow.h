#include <libfilepicker/FilePicker.h>
#include <libgraphic/font/FontFace.h>
#include <libgraphic/font/Parser.h>
#include <libio/Streams.h>
#include <libwidget/Components.h>
#include <libwidget/Layouts.h>
#include <libwidget/Window.h>

using namespace Widget;

struct FontManagerWindow : public Window
{
private:
    RefPtr<Graphic::SuperCoolFont::FontFace> _font_face;

public:
    FontManagerWindow(String path) : Window(WINDOW_RESIZABLE)
    {
        size(Math::Vec2i(700, 500));
        load_font(path);
    }

    void load_font(String path)
    {
        if (!path.null_or_empty())
        {
            IO::logln("Opening font from {}", path);
            should_rebuild();
        }
    }

    RefPtr<Element> build() override
    {
        return vflow({
            titlebar(Graphic::Icon::get("font-format"), "Font Manager"),
        });
    }
};
