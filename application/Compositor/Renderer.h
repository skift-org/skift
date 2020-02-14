#pragma once

#include <libgraphic/shape.h>

void renderer_initialize(void);

void renderer_region(Rectangle region);

Rectangle renderer_bound(void);

void renderer_blit(void);
