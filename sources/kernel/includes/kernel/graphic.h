#pragma once
#include <types.h>

void graphic_early_setup(uint width, uint height);
void graphic_setup();

void graphic_blit(uint *buffer);
void graphic_size(uint *width, uint *height);
void graphic_pixel(uint x, uint y, uint color);