#pragma once
#include "types.h"
#include "utils.h"
#include "kernel/paging.h"

uint virtual2physical(page_directorie_t * dir, uint virt);
void virtual_map(page_directorie_t * dir, u32 virt, u32 phys, bool user);
void virtual_unmap(page_directorie_t * dir, u32 virt);
