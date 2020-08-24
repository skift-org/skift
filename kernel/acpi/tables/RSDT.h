#pragma once

#include "kernel/acpi/tables/SDTH.h"

struct __packed RSDT
{
    SDTH header;
    uint32_t childs[];

    SDTH *child(size_t index)
    {
        return reinterpret_cast<SDTH *>(childs[index]);
    }

    size_t child_count()
    {
        return (header.Length - sizeof(header)) / 4;
    }
};
