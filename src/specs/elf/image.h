#pragma once

#include <karm-base/cursor.h>
#include <karm-base/enum.h>
#include <karm-base/string.h>

namespace Elf {

struct [[gnu::packed]] SectionHeader {
    u32 name;
    u32 type;
    u64 flags;

    u64 addr;
    u64 offset;
    u64 size;
    u32 link;
    u32 info;
    u64 addralign;
    u64 entsize;
};

enum struct ProgramType : u32 {
    NONE = 0,
    LOAD = 1,
    DYNAMIC = 2,
    INTERP = 3,
    NOTE = 4,
};

enum struct ProgramFlags : u32 {
    NIL = 0,
    READ = 1,
    WRITE = 2,
    EXEC = 4,
    SHARED = 32,
    PHDR = 64,
    TLS = 67,
};

FlagsEnum$(ProgramFlags);

struct [[gnu::packed]] ProgramHeader {
    ProgramType type;
    ProgramFlags flags;
    u64 offset;
    u64 vaddr;
    u64 paddr;
    u64 filesz;
    u64 memsz;
    u64 align;
};

struct [[gnu::packed]] Symbol {
    u32 name;
    u8 info;
    u8 other;
    u16 shndx;
    u64 value;
    u64 size;
};

struct [[gnu::packed]] Ident {
    u8 magic[4];
    u8 klass;
    u8 data;
    u8 version;
    u8 os_abi;
    u8 abi_version;
    u8 padding[7];
};

struct [[gnu::packed]] ImageHeader {
    Ident ident;

    u16 type;
    u16 machine;
    u32 version;

    u64 entry;

    u64 phoff;
    u64 shoff;

    u32 flags;
    u16 ehsize;

    u16 phentsize;
    u16 phnum;

    u16 shentsize;
    u16 shnum;

    u16 shstrndx;

    SectionHeader* sectionAt(usize index) {
        usize offset = shoff + index * shentsize;
        return (SectionHeader*)((u8*)this + offset);
    }

    ProgramHeader* programAt(usize index) {
        usize offset = phoff + index * phentsize;
        return (ProgramHeader*)((u8*)this + offset);
    }

    Str stringAt(usize offset) {
        if (offset == 0) {
            return "<null>";
        }
        return Str{(char const*)this + sectionAt(shstrndx)->offset + offset};
    }
};

struct Section {
    ImageHeader* _base;
    SectionHeader* _header;

    Str name() {
        return _base->stringAt(_header->name);
    }

    void* buf() {
        return (void*)((u8 const*)_base + _header->offset);
    }

    usize size() {
        return _header->size;
    }

    template <typename T>
    T const* as() {
        return static_cast<T const*>(buf());
    }

    template <typename T>
    MutSlice<T> slice() {
        return MutSlice<T>{(T*)buf(), size() / sizeof(T)};
    }

    template <typename T>
    Cursor<T> cursor() {
        return Cursor<T>{(T*)buf(), size() / sizeof(T)};
    }
};

struct Program {
    ImageHeader* _base;
    ProgramHeader* _header;

    using enum ProgramType;
    using Type = ProgramType;

    using enum ProgramFlags;
    using Flags = ProgramFlags;

    Type type() const {
        return _header->type;
    }

    Flags flags() const {
        return _header->flags;
    }

    usize offset() const {
        return _header->offset;
    }

    void* buf() {
        return (void*)((u8*)_base + _header->offset);
    }

    void const* buf() const {
        return (void const*)((u8 const*)_base + _header->offset);
    }

    Bytes bytes() const {
        return Bytes{
            (Byte*)buf(),
            static_cast<usize>(_header->filesz),
        };
    }

    usize filez() const {
        return _header->filesz;
    }

    usize vaddr() const {
        return _header->vaddr;
    }

    usize memsz() const {
        return _header->memsz;
    }
};

struct Image {
    Bytes _buf;

    Image(Bytes cursor)
        : _buf(cursor) {}

    bool valid() const {
        return sizeOf(_buf) >= sizeof(ImageHeader) and
               _buf[0] == '\x7f' and
               _buf[1] == 'E' and
               _buf[2] == 'L' and
               _buf[3] == 'F';
    }

    ImageHeader& header() {
        return *(ImageHeader*)_buf.buf();
    }

    auto sections() {
        return Iter([header = &header(), index = 0] mutable -> Opt<Section> {
            if (index >= header->shnum)
                return NONE;
            auto section = Section{header, header->sectionAt(index)};
            index++;
            return section;
        });
    }

    Opt<Section> sectionByName(Str name) {
        for (auto& section : sections()) {
            if (section.name() == name)
                return section;
        }

        return NONE;
    }

    auto programs() {
        return Iter([header = &header(), index = 0] mutable -> Opt<Program> {
            if (index >= header->phnum) {
                return NONE;
            }

            auto program = Program{header, header->programAt(index)};
            index++;
            return program;
        });
    }

    ProgramHeader* programAt(usize index) {
        return (ProgramHeader*)(_buf.buf() + header().phoff + index * header().phentsize);
    }
};

} // namespace Elf
