#pragma once

#include <libwidget/Widget.h>

typedef enum
{
    IMAGE_CENTER,
    IMAGE_STRETCH,
} ImageSizeMode;

typedef struct
{
    Widget widget;

    ImageSizeMode size_mode;
    Bitmap *bitmap;
} Image;

void image_set_image(Widget *image, const char *path);

Widget *image_create(Widget *parent, const char *path);
