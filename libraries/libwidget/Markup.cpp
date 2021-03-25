#include <string.h>

#include <libmarkup/Markup.h>
#include <libutils/NumberParser.h>
#include <libutils/Scanner.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Image.h>
#include <libwidget/Label.h>
#include <libwidget/Markup.h>
#include <libwidget/Panel.h>
#include <libwidget/Placeholder.h>
#include <libwidget/Slider.h>
#include <libwidget/TitleBar.h>

static void whitespace(Scanner &scan)
{
    scan.eat(" ");
}

static int number(Scanner &scan)
{
    int number = 0;

    while (scan.current_is("0123456789"))
    {
        number *= 10;
        number += scan.current() - '0';
        scan.foreward();
    }

    return number;
}

static Layout layout_parse(const char *string)
{
    if (!string)
    {
        return STACK();
    }

    Layout result = STACK();

    StringScanner scan(string, strlen(string));

    if (scan.skip_word("stack"))
    {
        result = STACK();
    }

    if (scan.skip_word("grid"))
    {
        scan.skip('(');

        whitespace(scan);

        int hcell = number(scan);

        whitespace(scan);

        scan.skip(',');

        whitespace(scan);

        int vcell = number(scan);

        whitespace(scan);

        scan.skip(',');

        whitespace(scan);

        int hspacing = number(scan);

        whitespace(scan);

        scan.skip(',');

        whitespace(scan);

        int vspacing = number(scan);

        result = GRID(hcell, vcell, hspacing, vspacing);
    }

    if (scan.skip_word("vgrid"))
    {
        scan.skip('(');
        whitespace(scan);
        int spacing = number(scan);
        result = VGRID(spacing);
    }

    if (scan.skip_word("hgrid"))
    {
        scan.skip('(');
        whitespace(scan);
        int spacing = number(scan);
        result = HGRID(spacing);
    }

    if (scan.skip_word("vflow"))
    {
        scan.skip('(');
        whitespace(scan);

        int spacing = number(scan);
        result = VFLOW(spacing);
    }

    if (scan.skip_word("hflow"))
    {
        scan.skip('(');
        whitespace(scan);

        int spacing = number(scan);
        result = HFLOW(spacing);
    }

    return result;
}

Insetsi insets_parse(const char *string)
{
    if (!string)
    {
        return Insetsi(0);
    }

    StringScanner scan{string, strlen(string)};

    if (!scan.skip_word("insets"))
    {
        return Insetsi(0);
    }

    scan.skip('(');
    whitespace(scan);

    int count;
    int args[4];

    for (count = 0; count < 4 && scan.current_is("0123456789"); count++)
    {
        args[count] = number(scan);
        whitespace(scan);
        scan.skip(',');
        whitespace(scan);
    }

    Insetsi result = Insetsi(0);

    if (count == 1)
    {
        result = Insetsi(args[0]);
    }
    else if (count == 2)
    {
        result = Insetsi(args[0], args[1]);
    }
    else if (count == 3)
    {
        result = Insetsi(args[0], args[1], args[2]);
    }
    else if (count == 4)
    {
        result = Insetsi(args[0], args[1], args[2], args[3]);
    }

    return result;
}

Anchor anchor_parse(const char *string)
{
    if (strcmp(string, "left") == 0)
    {
        return Anchor::LEFT;
    }
    if (strcmp(string, "center") == 0)
    {
        return Anchor::CENTER;
    }
    if (strcmp(string, "right") == 0)
    {
        return Anchor::RIGHT;
    }
    if (strcmp(string, "top_left") == 0)
    {
        return Anchor::TOP_LEFT;
    }
    if (strcmp(string, "top_center") == 0)
    {
        return Anchor::TOP_CENTER;
    }
    if (strcmp(string, "top_right") == 0)
    {
        return Anchor::TOP_RIGHT;
    }
    if (strcmp(string, "bottom_left") == 0)
    {
        return Anchor::BOTTOM_LEFT;
    }
    if (strcmp(string, "bottom_center") == 0)
    {
        return Anchor::BOTTOM_CENTER;
    }
    if (strcmp(string, "bottom_right") == 0)
    {
        return Anchor::BOTTOM_RIGHT;
    }

    return Anchor::LEFT;
}

static BitmapScaling scaling_parse(String string)
{
    if (string == "center")
    {
        return BitmapScaling::CENTER;
    }

    if (string == "cover")
    {
        return BitmapScaling::COVER;
    }

    if (string == "fit")
    {
        return BitmapScaling::FIT;
    }

    if (string == "stretch")
    {
        return BitmapScaling::STRETCH;
    }

    return BitmapScaling::CENTER;
}

void widget_apply_attribute_from_markup(Widget *widget, markup::Node &node)
{
    node.with_attribute("id", [&](auto &id) {
        widget->id(id);
    });

    node.with_attribute("layout", [&](auto &layout) {
        widget->layout(layout_parse(layout.cstring()));
    });

    node.with_attribute("padding", [&](auto &insets) {
        widget->insets(insets_parse(insets.cstring()));
    });

    if (node.has_attribute("fill"))
    {
        widget->flags(Widget::FILL);
    }
}

Widget *widget_create_from_markup(Widget *parent, markup::Node &node)
{
    Widget *widget = nullptr;

    if (node.is("Container"))
    {
        widget = new Container(parent);
    }

    if (node.is("Panel"))
    {
        if (node.has_attribute("rounded"))
        {
            auto panel = new Panel(parent);
            panel->border_radius(6);
            widget = panel;
        }
        else
        {
            widget = new Panel(parent);
        }
    }

    if (node.is("Button"))
    {
        Button::Style button_style = Button::TEXT;

        if (node.has_attribute("filled"))
        {
            button_style = Button::FILLED;
        }

        if (node.has_attribute("outlined"))
        {
            button_style = Button::OUTLINE;
        }

        if (node.has_attribute("text") && node.has_attribute("icon"))
        {
            widget = new Button(
                parent,
                button_style,
                Icon::get(node.get_attribute_or_default("icon", "duck")),
                node.get_attribute_or_default("text", "Button"));
        }
        else if (node.has_attribute("text"))
        {
            widget = new Button(
                parent,
                button_style,
                node.get_attribute_or_default("text", "Button"));
        }
        else if (node.has_attribute("icon"))
        {
            widget = new Button(
                parent,
                button_style,
                Icon::get(node.get_attribute_or_default("icon", "duck")));
        }
        else
        {
            widget = new Button(
                parent,
                button_style);
        }
    }

    if (node.is("Label"))
    {
        widget = new Label(
            parent,
            node.get_attribute_or_default("text", "Label"),
            anchor_parse(node.get_attribute_or_default("anchor", "left")
                             .cstring()));
    }

    if (node.is("Image"))
    {
        auto bitmap = Bitmap::load_from_or_placeholder(node.get_attribute_or_default("path", "null"));

        widget = new Image(parent, bitmap, scaling_parse(node.get_attribute_or_default("scaling", "center")));
    }

    if (node.is("Slider"))
    {
        widget = new Slider(parent);
    }

    if (node.is("TitleBar"))
    {
        widget = new TitleBar(parent);
    }

    if (widget == nullptr)
    {
        widget = new Placeholder(parent, node.type());
    }

    widget_apply_attribute_from_markup(widget, node);

    return widget;
}

void widget_create_childs_from_markup(Widget *parent, markup::Node &node)
{
    node.foreach_child([&](auto &child) {
        auto widget = widget_create_from_markup(parent, child);
        widget_create_childs_from_markup(widget, child);

        return Iteration::CONTINUE;
    });
}

WindowFlag window_flags_from_markup(markup::Node &node)
{
    WindowFlag flags = 0;

    if (node.has_attribute("borderless"))
    {
        flags |= WINDOW_BORDERLESS;
    }

    if (node.has_attribute("resizable"))
    {
        flags |= WINDOW_RESIZABLE;
    }

    if (node.has_attribute("always-focused"))
    {
        flags |= WINDOW_ALWAYS_FOCUSED;
    }

    if (node.has_attribute("transparent"))
    {
        flags |= WINDOW_TRANSPARENT;
    }

    return flags;
}

Window *window_create_from_markup(markup::Node &node)
{
    auto window = new Window(window_flags_from_markup(node));

    int width = parse_int_inline(PARSER_DECIMAL, node.get_attribute("width").cstring(), 250);

    int height = parse_int_inline(PARSER_DECIMAL, node.get_attribute("height").cstring(), 250);

    window->size(Vec2i(width, height));

    node.with_attribute("icon", [&](auto &icon) {
        window->icon(Icon::get(icon));
    });

    node.with_attribute("title", [&](auto &title) {
        window->title(title);
    });

    return window;
}

Window *window_create_from_file(String path)
{
    auto root = markup::parse_file(path);

    auto window = window_create_from_markup(root);

    widget_apply_attribute_from_markup(window->root(), root);
    widget_create_childs_from_markup(window->root(), root);

    return window;
}

Widget *widget_create_from_file(Widget *parent, String path)
{
    auto root = markup::parse_file(path);

    auto widget = widget_create_from_markup(parent, root);
    widget_create_childs_from_markup(widget, root);

    return widget;
}