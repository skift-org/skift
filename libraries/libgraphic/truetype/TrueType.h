#pragma once

#include <libsystem/Common.h>
#include <libsystem/utils/Slice.h>

typedef __big_endian struct
{
    uint8_t tag[4];
    uint32_t checksum;
    uint32_t offset;
    uint32_t length;
} TrueTypeTable;

typedef __big_endian struct
{
    uint32_t version;

    uint16_t table_count;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;

    TrueTypeTable tables[];
} TrueTypeHeader;

static inline bool truetype_tag_match(TrueTypeTable *table, const char *tag)
{
    for (size_t i = 0; i < 4; i++)
    {
        if (table->tag[i] != tag[i])
        {
            return false;
        }
    }

    return true;
}

uint32_t truetype_table_checksum(Slice table)
{
    uint32_t checksum = 0L;

    uint32_t *table_start = (uint32_t *)table.data;
    uint32_t *table_end = table_start + ((table.size + 3) & ~3) / sizeof(uint32_t);

    while (table_start < table_end)
    {
        checksum += *table_start++;
    }

    return checksum;
}

static inline Slice truetype_get_table(Slice fontdata, const char *tag)
{
    TrueTypeHeader *header = (TrueTypeHeader *)fontdata.data;

    for (size_t i = 0; i < header->table_count; i++)
    {
        TrueTypeTable *current_table = &header->tables[i];

        Slice table_slice = slice_sub_slice(fontdata, current_table->offset, current_table->length);

        if (truetype_tag_match(current_table, tag) &&
            !slice_is_empty(table_slice) &&
            current_table->checksum == truetype_table_checksum(table_slice))
        {
            return table_slice;
        }
    }

    return SLICE_NULL;
}
