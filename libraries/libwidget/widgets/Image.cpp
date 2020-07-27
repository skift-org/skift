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
            destination = bitmap_bound(widget->bitmap).centered_within(widget_get_bound(widget));
        }

        painter.blit_bitmap(widget->bitmap, bitmap_bound(widget->bitmap), destination);
    }
}

Vec2i image_size(Image *widget)
{
    if (widget->bitmap)
    {
        return bitmap_bound(widget->bitmap).size();
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
        bitmap_destroy(widget->bitmap);
    }
}

void image_set_image(Widget *image, const char *path)
{
    bitmap_destroy(((Image *)image)->bitmap);
    ((Image *)image)->bitmap = bitmap_load_from(path);
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

    image->bitmap = bitmap_load_from(path);
    image->size_mode = IMAGE_CENTER;

    widget_initialize(WIDGET(image), &image_class, parent);

    return WIDGET(image);
}
