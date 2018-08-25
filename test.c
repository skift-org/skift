#include "sources/libutils/includes/ds/list.h"
#include "sources/libutils/sources/list.c"

int main(int argc, char const *argv[])
{
    list_t * list = list_alloc();

    for(size_t i = 0; i < 10; i++)
    {
        list_push(list, i);
    }
    list_print(list);

    for(size_t i = 0; i < 10; i++)
        list_remove(list, i);

    list_print(list);
    list_free(list);
    return 0;
}
