#include <libgraphic/truetype/TrueType.h>
#include <libsystem/CString.h>
#include <libsystem/io/File.h>
#include <libsystem/io/Stream.h>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        return -1;
    }

    void *buffer = NULL;
    size_t size = 0;

    if (file_read_all(argv[1], &buffer, &size) != SUCCESS)
    {
        return -1;
    }

    TrueTypeHeader *header = (TrueTypeHeader *)buffer;

    printf("Version: 0x%08x\n", header->version);
    printf("Tables: %d\n", header->table_count);

    for (size_t i = 0; i < header->table_count; i++)
    {
        char tag_string[5];
        strlcpy(tag_string, (char *)header->tables[i].tag, 5);
        printf("- Table '%4s'\n", tag_string);
    }

    return 0;
}
