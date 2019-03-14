#pragma once

#include "hideo/node.h"

typedef struct
{
    const char* name;
    hideo_node_t* root;
} hideo_desktop_t;