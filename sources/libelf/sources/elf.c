#include "libelf.h"

int check_magic(raw_ELF_header_t * header)
{
    char * magic = (char*)&header->magic;

    return magic[0] == ELFMAG0 &&
           magic[1] == ELFMAG1 &&
           magic[2] == ELFMAG2 &&
           magic[3] == ELFMAG3 &&
           magic[4] == 1 && /* is 32bit */
           magic[5] == 1;
}

int elf_valid(void * file)
{
    raw_ELF_header_t * header = (raw_ELF_header_t *) file;

    return check_magic(header) &&
           (header->type == ELF_REL || ELF_EXEC) &&
           header->version == 1 &&
           header->machine == 3; // 386
}

int elf_read_header(ELF_t * header, void * file)
{
    raw_ELF_header_t * h = (raw_ELF_header_t *) file;

    header->valid = elf_valid(file);

    if (header->valid )
    {
        header->type = h->type;
        header->entry = h->entry;

        return 1;
    }
    else
    {
        header->type = ELF_UNKOWN;
        return 0;
    }
}

int elf_read_section(ELF_section_t * section, void * file,int index)
{
    raw_ELF_header_t * h = (raw_ELF_header_t *) file;
    raw_ELF_section_t * s = (raw_ELF_section_t *)(file + h->phoff + sizeof(raw_ELF_header_t) * index);

    if (index >= h->phnum) return 0;

    section->infile = (void*)((uint)file + s->offset);
    section->address = s->vaddr;
    section->size = s->filesz;
    section->memsize = s->memsz;

    return 1;
}
