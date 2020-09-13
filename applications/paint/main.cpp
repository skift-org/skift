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
    RefPtr<PaintDocument> document;

    /// --- Toolbar --- ///
    Widget *open_document;
    Widget *save_document;
    Widget *new_document;

    Widget *pencil;
    Widget *brush;
    Widget *eraser;
    Widget *fill;
    Widget *picker;

    Widget *insert_text;
    Widget *insert_line;
    Widget *insert_rectangle;
    Widget *insert_circle;

    Panel *primary_color;
    Panel *secondary_color;

    /// --- Canvas --- ///
    PaintCanvas *canvas;
};

static void update_toolbar(PaintWindow *window)
{
    window->primary_color->color(THEME_MIDDLEGROUND, window->document->primary_color());
    window->secondary_color->color(THEME_MIDDLEGROUND, window->document->secondary_color());
}

static void create_toolbar(PaintWindow *window, Widget *parent)
{
    Widget *toolbar = toolbar_create(parent);

    window->open_document = toolbar_icon_create(toolbar, Icon::get("folder-open"));
    window->save_document = toolbar_icon_create(toolbar, Icon::get("content-save"));
    window->new_document = toolbar_icon_create(toolbar, Icon::get("image-plus"));

    new Separator(toolbar);

    window->pencil = toolbar_icon_create(toolbar, Icon::get("pencil"));
    window->pencil->on(Event::ACTION, [window](auto) {
        window->canvas->tool(own<PencilTool>());
        update_toolbar(window);
    });

    window->brush = toolbar_icon_create(toolbar, Icon::get("brush"));
    window->brush->on(Event::ACTION, [window](auto) {
        window->canvas->tool(own<BrushTool>());
        update_toolbar(window);
    });

    window->eraser = toolbar_icon_create(toolbar, Icon::get("eraser"));
    window->eraser->on(Event::ACTION, [window](auto) {
        window->canvas->tool(own<EraserTool>());
        update_toolbar(window);
    });

    window->fill = toolbar_icon_create(toolbar, Icon::get("format-color-fill"));
    window->fill->on(Event::ACTION, [window](auto) {
        window->canvas->tool(own<FillTool>());
        update_toolbar(window);
    });

    window->picker = toolbar_icon_create(toolbar, Icon::get("eyedropper"));
    window->picker->on(Event::ACTION, [window](auto) {
        window->canvas->tool(own<PickerTool>());
        update_toolbar(window);
    });

    new Separator(toolbar);

    // TODO:
    window->insert_text = toolbar_icon_create(toolbar, Icon::get("format-text-variant"));
    window->insert_line = toolbar_icon_create(toolbar, Icon::get("vector-line"));
    window->insert_rectangle = toolbar_icon_create(toolbar, Icon::get("rectangle-outline"));
    window->insert_circle = toolbar_icon_create(toolbar, Icon::get("circle-outline"));

    new Separator(toolbar);

    Widget *primary_color_container = new Container(toolbar);
    primary_color_container->min_width(22);
    primary_color_container->insets(Insets(4));

    window->primary_color = new Panel(primary_color_container);
    window->primary_color->border_radius(4);
    window->primary_color->color(THEME_MIDDLEGROUND, window->document->primary_color());

    Widget *secondary_color_container = new Container(toolbar);
    secondary_color_container->min_width(22);
    secondary_color_container->insets(Insets(4));

    window->secondary_color = new Panel(secondary_color_container);
    window->secondary_color->border_radius(4);
    window->secondary_color->color(THEME_MIDDLEGROUND, window->document->secondary_color());
}

static void create_color_palette(PaintWindow *window, Widget *parent)
{
    __unused(window);

    Widget *palette = toolbar_create(parent);
    palette->layout(HFLOW(4));

    for (size_t i = 0; i < 18; i++)
    {
        Color color = _color_palette[i];

        auto color_widget = new Panel(palette);
        color_widget->border_radius(4);
        color_widget->min_width(30);
        color_widget->color(THEME_MIDDLEGROUND, color);

        color_widget->on(Event::MOUSE_BUTTON_PRESS, [window, color](auto event) {
            if (event->mouse.button == MOUSE_BUTTON_LEFT)
            {
                window->document->primary_color(color);
            }
            else if (event->mouse.button == MOUSE_BUTTON_RIGHT)
            {
                window->document->secondary_color(color);
            }

            update_toolbar(window);
        });
    }
}

static Window *paint_create_window(RefPtr<PaintDocument> document)
{
    PaintWindow *window = __create(PaintWindow);

    window_initialize(window, WINDOW_RESIZABLE);

    window->icon(Icon::get("brush"));
    window->title("Paint");
    window->size(Vec2i(600, 560));

    window->document = document;

    Widget *root = window->root();
    root->layout(VFLOW(0));

    create_toolbar(window, root);

    window->canvas = new PaintCanvas(root, window->document);
    window->canvas->attributes(LAYOUT_FILL);

    create_color_palette(window, root);

    document->on_color_change = [window]() {
        update_toolbar(window);
    };

    return window;
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    auto bitmap = Bitmap::create_shared(400, 400).take_value();
    bitmap->clear(Colors::BLACKTRANSPARENT);
    auto document = make<PaintDocument>(bitmap);
    paint_create_window(document)->show();

    return application_run();
}
