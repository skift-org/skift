#pragma once

#include <libsystem/core/CString.h>

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

    SDTH *child(const char *signature)
    {
        for (size_t i = 0; i < child_count(); i++)
        {
            SDTH *sdth = child(i);

            if (memcmp(sdth->Signature, signature, 4) == 0)
            {
                return sdth;
            }
        }

        return nullptr;
    }
};
