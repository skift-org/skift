#include <assert.h>

#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Panel.h>
#include <libwidget/Separator.h>
#include <libwidget/TitleBar.h>

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

struct PaintWindow : public Window
{
private:
    RefPtr<PaintDocument> _document;

    /// --- Toolbar --- ///
    Component *_open_document;
    Component *_save_document;
    Component *_new_document;

    Component *_pencil;
    Component *_brush;
    Component *_eraser;
    Component *_fill;
    Component *_picker;

    Component *_insert_text;
    Component *_insert_line;
    Component *_insert_rectangle;
    Component *_insert_circle;

    Panel *_primary_color;
    Panel *_secondary_color;

    /// --- Canvas --- ///
    PaintCanvas *_canvas;

public:
    PaintWindow(RefPtr<PaintDocument> document) : Window(WINDOW_RESIZABLE)
    {
        icon(Graphic::Icon::get("brush"));
        title("Paint");
        size(Vec2i(600, 560));

        _document = document;

        root()->layout(VFLOW(0));

        new TitleBar(root());

        create_toolbar(root());

        _canvas = new PaintCanvas(root(), document);
        _canvas->flags(Component::FILL);

        create_color_palette(root());

        document->on_color_change = [this]() {
            update_toolbar();
        };
    }

    void create_toolbar(Component *parent)
    {
        auto toolbar = new Panel(parent);

        toolbar->layout(HFLOW(4));
        toolbar->insets(Insetsi(4, 4));
        toolbar->max_height(38);
        toolbar->min_height(38);

        _open_document = new Button(toolbar, Button::TEXT, Graphic::Icon::get("folder-open"));
        _save_document = new Button(toolbar, Button::TEXT, Graphic::Icon::get("content-save"));
        _new_document = new Button(toolbar, Button::TEXT, Graphic::Icon::get("image-plus"));

        new Separator(toolbar);

        _pencil = new Button(toolbar, Button::TEXT, Graphic::Icon::get("pencil"));
        _pencil->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<PencilTool>());
            update_toolbar();
        });

        _brush = new Button(toolbar, Button::TEXT, Graphic::Icon::get("brush"));
        _brush->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<BrushTool>());
            update_toolbar();
        });

        _eraser = new Button(toolbar, Button::TEXT, Graphic::Icon::get("eraser"));
        _eraser->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<EraserTool>());
            update_toolbar();
        });

        _fill = new Button(toolbar, Button::TEXT, Graphic::Icon::get("format-color-fill"));
        _fill->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<FillTool>());
            update_toolbar();
        });

        _picker = new Button(toolbar, Button::TEXT, Graphic::Icon::get("eyedropper"));
        _picker->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<PickerTool>());
            update_toolbar();
        });

        new Separator(toolbar);

        // TODO:
        _insert_text = new Button(toolbar, Button::TEXT, Graphic::Icon::get("format-text-variant"));
        _insert_line = new Button(toolbar, Button::TEXT, Graphic::Icon::get("vector-line"));
        _insert_rectangle = new Button(toolbar, Button::TEXT, Graphic::Icon::get("rectangle-outline"));
        _insert_circle = new Button(toolbar, Button::TEXT, Graphic::Icon::get("circle-outline"));

        new Separator(toolbar);

        Component *primary_color_container = new Container(toolbar);
        primary_color_container->insets(Insetsi(4));
        primary_color_container->flags(Component::SQUARE);

        _primary_color = new Panel(primary_color_container);
        _primary_color->border_radius(4);
        _primary_color->color(THEME_MIDDLEGROUND, _document->primary_color());

        Component *secondary_color_container = new Container(toolbar);
        secondary_color_container->insets(Insetsi(4));
        secondary_color_container->flags(Component::SQUARE);

        _secondary_color = new Panel(secondary_color_container);
        _secondary_color->border_radius(4);
        _secondary_color->color(THEME_MIDDLEGROUND, _document->secondary_color());
    }

    void create_color_palette(Component *parent)
    {
        auto palette = new Panel(parent);

        palette->layout(HFLOW(4));
        palette->insets(Insetsi(4, 4));
        palette->max_height(38);
        palette->min_height(38);

        palette->layout(HFLOW(4));

        for (size_t i = 0; i < __array_length(_color_palette); i++)
        {
            Graphic::Color color = _color_palette[i];

            auto color_widget = new Panel(palette);
            color_widget->border_radius(4);
            color_widget->min_width(30);
            color_widget->color(THEME_MIDDLEGROUND, color);

            color_widget->on(Event::MOUSE_BUTTON_PRESS, [this, color](auto event) {
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
        _primary_color->color(THEME_MIDDLEGROUND, _document->primary_color());
        _secondary_color->color(THEME_MIDDLEGROUND, _document->secondary_color());
    }
};

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    auto bitmap = Graphic::Bitmap::create_shared(400, 400).take_value();
    bitmap->clear(Graphic::Colors::BLACKTRANSPARENT);

    auto document = make<PaintDocument>(bitmap);

    auto window = new PaintWindow(document);
    window->show();

    return Application::run();
}
