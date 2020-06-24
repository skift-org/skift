#include <libgraphic/truetype/Cmap.h>
#include <libgraphic/truetype/TrueType.h>

static bool truetype_tag_match(TrueTypeTable *table, const char *tag)
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

static uint32_t truetype_table_checksum(Slice table)
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

Slice truetype_get_table(Slice fontdata, const char *tag)
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

// uint32_t truetype_get_glyph_index(Slice fontdata, Codepoint codepoint)
// {
//     Slice cmap_table = truetype_get_table(fontdata, TRUETYPE_CMAP_TABLE_NAME);
//
//     if (slice_is_empty(cmap_table))
//     {
//         return 0;
//     }
//
//
// }
