#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Shape.h>

void renderer_initialize();

Rectangle renderer_bound();

void renderer_region_dirty(Rectangle region);

void renderer_repaint_dirty();

bool renderer_set_resolution(int width, int height);

void renderer_set_wallaper(RefPtr<Bitmap> wallaper);
