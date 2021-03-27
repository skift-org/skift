#pragma once

#include <libgraphic/Bitmap.h>
#include <libutils/Rect.h>

void renderer_initialize();

Recti renderer_bound();

void renderer_region_dirty(Recti region);

void renderer_repaint_dirty();

bool renderer_set_resolution(int width, int height);

void renderer_set_wallaper(RefPtr<Graphic::Bitmap> wallaper);
