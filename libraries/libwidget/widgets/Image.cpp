#include <libgraphic/Painter.h>
#include <libwidget/widgets/Image.h>

void image_paint(Image *widget, Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget->bitmap)
    {
        Rectangle destination = widget_get_bound(widget);

        if (widget->size_mode == IMAGE_CENTER)
        {
            destination = widget->bitmap->bound().centered_within(widget_get_bound(widget));
        }

        painter.blit_bitmap(*widget->bitmap, widget->bitmap->bound(), destination);
    }
}

Vec2i image_size(Image *widget)
{
    if (widget->bitmap)
    {
        return widget->bitmap->bound().size();
    }
    else
    {
        return widget_get_bound(widget).size();
    }
}

void image_destroy(Image *widget)
{
    if (widget->bitmap)
    {
        widget->bitmap = nullptr;
    }
}

void image_set_image(Widget *image, const char *path)
{
    ((Image *)image)->bitmap = Bitmap::load_from_or_placeholder(path);
}

static const WidgetClass image_class = {
    .name = "Image",

    .destroy = (WidgetDestroyCallback)image_destroy,
    .paint = (WidgetPaintCallback)image_paint,
    .size = (WidgetComputeSizeCallback)image_size,
};

Widget *image_create(Widget *parent, const char *path)
{
    Image *image = __create(Image);

    image->bitmap = Bitmap::load_from_or_placeholder(path);
    image->size_mode = IMAGE_CENTER;

    widget_initialize(image, &image_class, parent);

    return image;
}
