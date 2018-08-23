#pragma once
#include "types.h"
#include "utils.h"

# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]
 
# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	(1)  // 32-bit Architecture

typedef PACKED(struct) 
{
        unsigned char magic[16];      /* ELF identification */
        u16 type;             /* 2 (exec file) */
        u16 machine;          /* 3 (intel architecture) */
        u32 version;          /* 1 */
        u32 entry;            /* starting point */
        u32 phoff;            /* program header table offset */
        u32 shoff;            /* section header table offset */
        u32 flags;            /* various flags */
        u16 ehsize;           /* ELF header (this) size */
        u16 phentsize;        /* program header table entry size */
        u16 phnum;            /* number of entries */
        u16 shentsize;        /* section header table entry size */
        u16 shnum;            /* number of entries */
        u16 shstrndx;         /* index of the section name string table */
} raw_ELF_header_t;

typedef PACKED(struct) 
{
        u32 type;             /* type of segment */
        u32 offset;
        u32 vaddr;
        u32 paddr;
        u32 filesz;
        u32 memsz;
        u32 flags;
        u32 align;
} raw_ELF_section_t;



typedef enum 
{
    ELF_UNKOWN = 0,
    ELF_REL = 1,
    ELF_EXEC = 2
} ELF_type_t;

typedef struct 
{
    char * name;

    void * infile;
    int address;

    int size;
    int memsize;
} ELF_section_t;

typedef struct
{
    bool valid;
    int type;
    int entry;
} ELF_t;


int elf_valid(void * elf);