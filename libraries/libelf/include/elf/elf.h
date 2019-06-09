#pragma once

#include <skift/runtime.h>

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define EI_NIDENT 16

typedef uint32_t Elf32_Word;
typedef uint32_t elf_addr_t;
typedef uint32_t elf_offset_t;
typedef uint32_t Elf32_Sword;
typedef uint16_t Elf32_Half;

typedef struct
{
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    elf_addr_t e_entry;
    elf_offset_t e_phoff;
    elf_offset_t e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Header;

#define ET_NONE 0       /* No file type */
#define ET_REL 1        /* Relocatable file */
#define ET_EXEC 2       /* Executable file */
#define ET_DYN 3        /* Shared object file */
#define ET_CORE 4       /* Core file */
#define ET_LOPROC 0xff0 /* [Processor Specific] */
#define ET_HIPROC 0xfff /* [Processor Specific] */

#define EM_NONE 0
#define EM_386 3

#define EV_NONE 0
#define EV_CURRENT 1

typedef struct
{
    Elf32_Word p_type;
    elf_offset_t p_offset;
    elf_addr_t p_vaddr;
    elf_addr_t p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7FFFFFFF

typedef struct
{
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    elf_addr_t sh_addr;
    elf_offset_t sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct
{
    uint32_t id;
    uintptr_t ptr;
} Elf32_auxv;

typedef struct
{
    Elf32_Word st_name;
    elf_addr_t st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct
{
    elf_addr_t r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct
{
    Elf32_Sword d_tag;
    union {
        Elf32_Word d_val;
        elf_addr_t d_ptr;
        elf_offset_t d_off;
    } d_un;
} Elf32_Dyn;

#define SHT_NONE 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_NOBITS 8
#define SHT_REL 9

#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s, t) (((s) << 8) + (unsigned char)(t))

#define ELF32_ST_BIND(i) ((i) >> 4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b, t) (((b) << 4) + ((t)&0xf))

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2
#define STB_NUM 3

#define STB_LOPROC 13
#define STB_HIPROC 15

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_COMMON 5
#define STT_TLS 6
#define STT_NUM 7

#define STT_LOPROC 13
#define STT_HIPROC 15
