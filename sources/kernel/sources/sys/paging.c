
#include <string.h>

#include "kernel/paging.h"
#include "kernel/physical.h"

page_table_t * paging_new_page_table()
{
    page_table_t * page_table = (page_table_t *)physical_alloc();
    memset(page_table, 0, sizeof(page_table_t));
    return page_table;
}

page_directorie_t * paging_new_page_directorie()
{
    page_directorie_t * page_directorie = (page_directorie_t *)physical_alloc();
    memset(page_directorie, 0, sizeof(page_directorie_t));
    return page_directorie;
}