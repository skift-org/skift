#include "kernel/filesystem.h"
#include "kernel/logging.h"
#include "libelf.h"

int exec(char * path)
{
    file_t* file = file_open(NULL, path);
    if (file == NULL)
    {
        debug("%s file not found!", path);
        return 1;
    }
    void * buffer = file_read_all(file);
    file_close(file);

    if (buffer != NULL)
    {
        ELF_t header;
        elf_read_header(&header, buffer);
        debug("ELF file: VALID=%d TYPE=%d ENTRY=0x%x", header.valid, header.type, header.entry);

        ELF_section_t section;
        for (size_t i = 0; elf_read_section(&section, buffer, i); i++) {
          debug("ELF section");
        }
    }

    free(buffer);

    return 0;
}
