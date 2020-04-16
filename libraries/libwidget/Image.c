#include <libgraphic/Painter.h>
#include <libwidget/Image.h>

void image_paint(Image *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget->bitmap)
    {
        Rectangle destination = widget_bound(widget);

        if (widget->size_mode == IMAGE_CENTER)
        {
            destination = rectangle_center_within(bitmap_bound(widget->bitmap), widget_bound(widget));
        }

        painter_blit_bitmap(painter, widget->bitmap, bitmap_bound(widget->bitmap), destination);
    }
}

Point image_size(Image *widget)
{
    if (widget->bitmap)
    {
        return bitmap_bound(widget->bitmap).size;
    }
    else
    {
        return widget_bound(widget).size;
    }
}

void image_destroy(Image *widget)
{
    if (widget->bitmap)
    {
        bitmap_destroy(widget->bitmap);
    }
}

Widget *image_create(Widget *parent, const char *path)
{
    Image *image = __create(Image);

    image->bitmap = bitmap_load_from(path);
    image->size_mode = IMAGE_CENTER;

    WIDGET(image)->paint = (WidgetPaintCallback)image_paint;
    WIDGET(image)->size = (WidgetComputeSizeCallback)image_size;
    WIDGET(image)->destroy = (WidgetDestroyCallback)image_destroy;

    widget_initialize(WIDGET(image), "Image", parent);

    return WIDGET(image);
}