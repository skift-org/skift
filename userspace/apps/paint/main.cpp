#include <assert.h>

#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Elements.h>

#include "paint/PaintCanvas.h"
#include "paint/PaintDocument.h"
#include "paint/PaintTool.h"

static Graphic::Color _color_palette[] = {
    Graphic::Color::from_hex(0x000000),
    Graphic::Color::from_hex(0x1a1c2c),
    Graphic::Color::from_hex(0x5d275d),
    Graphic::Color::from_hex(0xb13e53),
    Graphic::Color::from_hex(0xef7d57),
    Graphic::Color::from_hex(0xffcd75),
    Graphic::Color::from_hex(0xa7f070),
    Graphic::Color::from_hex(0x38b764),
    Graphic::Color::from_hex(0x257179),
    Graphic::Color::from_hex(0x29366f),
    Graphic::Color::from_hex(0x3b5dc9),
    Graphic::Color::from_hex(0x41a6f6),
    Graphic::Color::from_hex(0x73eff7),
    Graphic::Color::from_hex(0xffffff),
    Graphic::Color::from_hex(0xf4f4f4),
    Graphic::Color::from_hex(0x94b0c2),
    Graphic::Color::from_hex(0x566c86),
    Graphic::Color::from_hex(0x333c57),
};

struct PaintWindow : public Widget::Window
{
private:
    RefPtr<PaintDocument> _document;

    /// --- Toolbar --- ///

    RefPtr<Widget::Element> _open_document;
    RefPtr<Widget::Element> _save_document;
    RefPtr<Widget::Element> _new_document;

    RefPtr<Widget::Element> _pencil;
    RefPtr<Widget::Element> _brush;
    RefPtr<Widget::Element> _eraser;
    RefPtr<Widget::Element> _fill;
    RefPtr<Widget::Element> _picker;

    RefPtr<Widget::Element> _insert_text;
    RefPtr<Widget::Element> _insert_line;
    RefPtr<Widget::Element> _insert_rectangle;
    RefPtr<Widget::Element> _insert_circle;

    RefPtr<Widget::Element> _primary_color;
    RefPtr<Widget::Element> _secondary_color;

    /// --- Canvas --- ///

    RefPtr<PaintCanvas> _canvas;

public:
    PaintWindow(RefPtr<PaintDocument> document) : Window(WINDOW_RESIZABLE)
    {
        size(Math::Vec2i(600, 560));

        _document = document;

        root()->add(Widget::titlebar(Graphic::Icon::get("brush"), "Paint"));

        create_toolbar(root());

        _canvas = root()->add<PaintCanvas>(document);
        _canvas->flags(Widget::Element::FILL);

        create_color_palette(root());

        document->on_color_change = [this]() {
            update_toolbar();
        };
    }

    void create_toolbar(RefPtr<Widget::Element> parent)
    {
        auto toolbar = parent->add(Widget::panel());

        _open_document = toolbar->add(Widget::basic_button(Graphic::Icon::get("folder-open")));
        _save_document = toolbar->add(Widget::basic_button(Graphic::Icon::get("content-save")));
        _new_document = toolbar->add(Widget::basic_button(Graphic::Icon::get("image-plus")));

        toolbar->add(Widget::separator());

        _pencil = toolbar->add(Widget::basic_button(Graphic::Icon::get("pencil"), [this] {
            _canvas->tool(own<PencilTool>());
            update_toolbar();
        }));

        _brush = toolbar->add(Widget::basic_button(Graphic::Icon::get("brush"), [this] {
            _canvas->tool(own<BrushTool>());
            update_toolbar();
        }));

        _eraser = toolbar->add(Widget::basic_button(Graphic::Icon::get("eraser"), [this] {
            _canvas->tool(own<EraserTool>());
            update_toolbar();
        }));

        _fill = toolbar->add(Widget::basic_button(Graphic::Icon::get("format-color-fill"), [this] {
            _canvas->tool(own<FillTool>());
            update_toolbar();
        }));

        _picker = toolbar->add(Widget::basic_button(Graphic::Icon::get("eyedropper"), [this] {
            _canvas->tool(own<PickerTool>());
            update_toolbar();
        }));

        toolbar->add(Widget::separator());

        // TODO:
        _insert_text = toolbar->add(Widget::basic_button(Graphic::Icon::get("format-text-variant")));
        _insert_line = toolbar->add(Widget::basic_button(Graphic::Icon::get("vector-line")));
        _insert_rectangle = toolbar->add(Widget::basic_button(Graphic::Icon::get("rectangle-outline")));
        _insert_circle = toolbar->add(Widget::basic_button(Graphic::Icon::get("circle-outline")));

        toolbar->add(Widget::separator());

        auto primary_color_container = toolbar->add<Widget::Element>();
        primary_color_container->flags(Widget::Element::SQUARE);

        _primary_color = primary_color_container->add(Widget::panel(4));
        _primary_color->color(Widget::THEME_MIDDLEGROUND, _document->primary_color());
        _primary_color->flags(Widget::Element::FILL);

        auto secondary_color_container = toolbar->add<Widget::Element>();
        secondary_color_container->flags(Widget::Element::SQUARE);

        _secondary_color = secondary_color_container->add(Widget::panel(4));
        _secondary_color->color(Widget::THEME_MIDDLEGROUND, _document->secondary_color());
        _secondary_color->flags(Widget::Element::FILL);
    }

    void create_color_palette(RefPtr<Widget::Element> parent)
    {
        auto palette = parent->add(Widget::panel());

        palette->max_height(38);
        palette->min_height(38);

        for (size_t i = 0; i < ARRAY_LENGTH(_color_palette); i++)
        {
            Graphic::Color color = _color_palette[i];

            auto color_widget = palette->add(Widget::panel(4));
            color_widget->min_width(30);
            color_widget->color(Widget::THEME_MIDDLEGROUND, color);

            color_widget->on(Widget::Event::MOUSE_BUTTON_PRESS, [this, color](auto event) {
                if (event->mouse.button == MOUSE_BUTTON_LEFT)
                {
                    _document->primary_color(color);
                }
                else if (event->mouse.button == MOUSE_BUTTON_RIGHT)
                {
                    _document->secondary_color(color);
                }

                update_toolbar();
            });
        }
    }

    void update_toolbar()
    {
        _primary_color->color(Widget::THEME_MIDDLEGROUND, _document->primary_color());
        _secondary_color->color(Widget::THEME_MIDDLEGROUND, _document->secondary_color());
    }
};

int main(int, char **)
{
    auto bitmap = Graphic::Bitmap::create_shared(400, 400).unwrap();
    bitmap->clear(Graphic::Colors::BLACKTRANSPARENT);

    auto document = make<PaintDocument>(bitmap);

    auto window = new PaintWindow(document);
    window->show();

    return Widget::Application::the().run();
}
