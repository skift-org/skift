#pragma once

#include <libwidget/Widget.h>

enum ImageSizeMode
{
    IMAGE_CENTER,
    IMAGE_STRETCH,
};

struct Image
{
    Widget widget;

    ImageSizeMode size_mode;
    RefPtr<Bitmap> bitmap;
};

void image_set_image(Widget *image, const char *path);

Widget *image_create(Widget *parent, const char *path);
