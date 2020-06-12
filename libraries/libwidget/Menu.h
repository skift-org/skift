#pragma once

#include <libgraphic/Bitmap.h>
#include <libsystem/utils/List.h>

typedef struct Menu Menu;

Menu *menu_create(Menu *parent, const char *icon, const char *text);

void menu_destroy(Menu *menu);

void menu_show(Menu *menu);
