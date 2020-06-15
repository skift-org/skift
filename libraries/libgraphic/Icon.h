#pragma once

#include <libgraphic/Bitmap.h>

#define ICON_SIZE_LIST(__ENTRY) \
    __ENTRY(18)                 \
    __ENTRY(24)                 \
    __ENTRY(36)                 \
    __ENTRY(48)

#define ICON_SIZE_ENUM_ENTRY(__size) ICON_##__size##PX,
typedef enum
{
    ICON_SIZE_LIST(ICON_SIZE_ENUM_ENTRY) __ICON_SIZE_COUNT,
} IconSize;

typedef struct
{
    char *name;
    Bitmap *sizes[__ICON_SIZE_COUNT];
} Icon;

Icon *icon_get(const char *name);

Rectangle icon_bound(Icon *icon, IconSize size);

Bitmap *icon_get_bitmap(Icon *icon, IconSize size);
