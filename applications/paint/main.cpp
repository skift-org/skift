#include <libsystem/Assert.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

#include "paint/PaintCanvas.h"
#include "paint/PaintDocument.h"
#include "paint/PaintTool.h"

static Color _color_palette[] = {
    Color::from_hex(0x000000),
    Color::from_hex(0x1a1c2c),
    Color::from_hex(0x5d275d),
    Color::from_hex(0xb13e53),
    Color::from_hex(0xef7d57),
    Color::from_hex(0xffcd75),
    Color::from_hex(0xa7f070),
    Color::from_hex(0x38b764),
    Color::from_hex(0x257179),
    Color::from_hex(0x29366f),
    Color::from_hex(0x3b5dc9),
    Color::from_hex(0x41a6f6),
    Color::from_hex(0x73eff7),
    Color::from_hex(0xffffff),
    Color::from_hex(0xf4f4f4),
    Color::from_hex(0x94b0c2),
    Color::from_hex(0x566c86),
    Color::from_hex(0x333c57),
};

struct PaintWindow : public Window
{
private:
    RefPtr<PaintDocument> _document;

    /// --- Toolbar --- ///
    Widget *_open_document;
    Widget *_save_document;
    Widget *_new_document;

    Widget *_pencil;
    Widget *_brush;
    Widget *_eraser;
    Widget *_fill;
    Widget *_picker;

    Widget *_insert_text;
    Widget *_insert_line;
    Widget *_insert_rectangle;
    Widget *_insert_circle;

    Panel *_primary_color;
    Panel *_secondary_color;

    /// --- Canvas --- ///
    PaintCanvas *_canvas;

public:
    PaintWindow(RefPtr<PaintDocument> document) : Window(WINDOW_RESIZABLE)
    {
        icon(Icon::get("brush"));
        title("Paint");
        size(Vec2i(600, 560));

        _document = document;

        root()->layout(VFLOW(0));

        create_toolbar(root());

        _canvas = new PaintCanvas(root(), document);
        _canvas->attributes(LAYOUT_FILL);

        create_color_palette(root());

        document->on_color_change = [this]() {
            update_toolbar();
        };
    }

    void create_toolbar(Widget *parent)
    {
        Widget *toolbar = toolbar_create(parent);

        _open_document = toolbar_icon_create(toolbar, Icon::get("folder-open"));
        _save_document = toolbar_icon_create(toolbar, Icon::get("content-save"));
        _new_document = toolbar_icon_create(toolbar, Icon::get("image-plus"));

        new Separator(toolbar);

        _pencil = toolbar_icon_create(toolbar, Icon::get("pencil"));
        _pencil->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<PencilTool>());
            update_toolbar();
        });

        _brush = toolbar_icon_create(toolbar, Icon::get("brush"));
        _brush->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<BrushTool>());
            update_toolbar();
        });

        _eraser = toolbar_icon_create(toolbar, Icon::get("eraser"));
        _eraser->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<EraserTool>());
            update_toolbar();
        });

        _fill = toolbar_icon_create(toolbar, Icon::get("format-color-fill"));
        _fill->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<FillTool>());
            update_toolbar();
        });

        _picker = toolbar_icon_create(toolbar, Icon::get("eyedropper"));
        _picker->on(Event::ACTION, [this](auto) {
            _canvas->tool(own<PickerTool>());
            update_toolbar();
        });

        new Separator(toolbar);

        // TODO:
        _insert_text = toolbar_icon_create(toolbar, Icon::get("format-text-variant"));
        _insert_line = toolbar_icon_create(toolbar, Icon::get("vector-line"));
        _insert_rectangle = toolbar_icon_create(toolbar, Icon::get("rectangle-outline"));
        _insert_circle = toolbar_icon_create(toolbar, Icon::get("circle-outline"));

        new Separator(toolbar);

        Widget *primary_color_container = new Container(toolbar);
        primary_color_container->insets(Insets(4));
        primary_color_container->attributes(LAYOUT_SQUARE);

        _primary_color = new Panel(primary_color_container);
        _primary_color->border_radius(4);
        _primary_color->color(THEME_MIDDLEGROUND, _document->primary_color());

        Widget *secondary_color_container = new Container(toolbar);
        secondary_color_container->insets(Insets(4));
        secondary_color_container->attributes(LAYOUT_SQUARE);

        _secondary_color = new Panel(secondary_color_container);
        _secondary_color->border_radius(4);
        _secondary_color->color(THEME_MIDDLEGROUND, _document->secondary_color());
    }

    void create_color_palette(Widget *parent)
    {
        Widget *palette = toolbar_create(parent);
        palette->layout(HFLOW(4));

        for (size_t i = 0; i < __array_length(_color_palette); i++)
        {
            Color color = _color_palette[i];

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
    application_initialize(argc, argv);

    auto bitmap = Bitmap::create_shared(400, 400).take_value();
    bitmap->clear(Colors::BLACKTRANSPARENT);

    auto document = make<PaintDocument>(bitmap);

    auto window = new PaintWindow(document);
    window->show();

    return application_run();
}
