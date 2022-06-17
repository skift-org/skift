#pragma once

#include <karm-base/cursor.h>
#include <karm-base/enum.h>
#include <karm-base/std.h>
#include <karm-base/string.h>

namespace Elf {

struct [[gnu::packed]] SectionHeader {
    uint32_t name;
    uint32_t type;
    uint64_t flags;

    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;
};

enum struct ProgramType : uint32_t {
    NONE = 0,
    LOAD = 1,
    DYNAMIC = 2,
    INTERP = 3,
    NOTE = 4,
};

enum struct ProgramFlags : uint32_t {
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
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
};

struct [[gnu::packed]] Symbol {
    uint32_t name;
    uint8_t info;
    uint8_t other;
    uint16_t shndx;
    uint64_t value;
    uint64_t size;
};

struct [[gnu::packed]] Ident {
    uint8_t magic[4];
    uint8_t klass;
    uint8_t data;
    uint8_t version;
    uint8_t os_abi;
    uint8_t abi_version;
    uint8_t padding[7];
};

struct [[gnu::packed]] ImageHeader {
    Ident ident;

    uint16_t type;
    uint16_t machine;
    uint32_t version;

    uint64_t entry;

    uint64_t phoff;
    uint64_t shoff;

    uint32_t flags;
    uint16_t ehsize;

    uint16_t phentsize;
    uint16_t phnum;

    uint16_t shentsize;
    uint16_t shnum;

    uint16_t shstrndx;

    SectionHeader *sectionAt(size_t index) {
        size_t offset = shoff + index * shentsize;
        return (SectionHeader *)((uint8_t *)this + offset);
    }

    ProgramHeader *programAt(size_t index) {
        size_t offset = phoff + index * phentsize;
        return (ProgramHeader *)((uint8_t *)this + offset);
    }

    Str stringAt(size_t offset) {
        if (offset == 0) {
            return "<null>";
        }
        return Str{(char const *)this + sectionAt(shstrndx)->offset + offset};
    }
};

struct Section {
    ImageHeader *_base;
    SectionHeader *_header;

    Str name() {
        return _base->stringAt(_header->name);
    }

    void *buf() {
        return (void *)((uint8_t const *)_base + _header->offset);
    }

    size_t size() {
        return _header->size;
    }

    template <typename T>
    T const *as() {
        return static_cast<T const *>(buf());
    }

    template <typename T>
    MutSlice<T> slice() {
        return MutSlice<T>{(T *)buf(), size() / sizeof(T)};
    }

    template <typename T>
    Cursor<T> cursor() {
        return Cursor<T>{(T *)buf(), size() / sizeof(T)};
    }
};

struct Program {
    ImageHeader *_base;
    ProgramHeader *_header;

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

    void *buf() {
        return (void *)((uint8_t const *)_base + _header->offset);
    }

    size_t filez() const {
        return _header->filesz;
    }

    size_t vaddr() const {
        return _header->vaddr;
    }

    size_t memsz() const {
        return _header->memsz;
    }
};

struct Image {
    Cursor<uint8_t> _cursor;

    Image(Cursor<uint8_t> cursor)
        : _cursor(cursor) {}

    bool valid() const {
        return _cursor.rem() >= sizeof(ImageHeader) &&
               _cursor[0] == '\x7f' &&
               _cursor[1] == 'E' &&
               _cursor[2] == 'L' &&
               _cursor[3] == 'F';
    }

    ImageHeader &header() {
        return *(ImageHeader *)_cursor.buf();
    }

    auto sections() {
        return Iter([header = &header(), index = 0]() mutable -> Opt<Section> {
            if (index >= header->shnum) {
                return NONE;
            }

            auto section = Section{header, header->sectionAt(index)};
            index++;
            return section;
        });
    }

    Opt<Section> sectionByName(Str name) {
        for (auto &section : sections()) {
            Str n = section.name();

            if (Op::eq(n, name)) {
                return section;
            }
        }

        return NONE;
    }

    auto programs() {
        return Iter([header = &header(), index = 0]() mutable -> Opt<Program> {
            if (index >= header->phnum) {
                return NONE;
            }

            auto program = Program{header, header->programAt(index)};
            index++;
            return program;
        });
    }

    ProgramHeader *programAt(size_t index) {
        return (ProgramHeader *)(_cursor.buf() + header().phoff + index * header().phentsize);
    }
};

} // namespace Elf
