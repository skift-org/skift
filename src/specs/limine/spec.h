#pragma once

#include <efi/guid.h>
#include <karm-base/array.h>
#include <karm-base/std.h>

namespace Limine {

static constexpr u64 MAGIC0 = 0xc7b1dd30df4c8b88;
static constexpr u64 MAGIC1 = 0x0a82e883a194f07b;

using Id = Array<u64, 4>;

struct Response {
    u64 rev;
};

template <typename T>
struct Ptr;

template <>
struct Ptr<void> {
    u64 _raw;

    template <typename T>
    operator T *() {
        return reinterpret_cast<T *>(_raw);
    }

    template <typename T>
    operator T const *() const {
        return reinterpret_cast<T const *>(_raw);
    }

    operator bool() const {
        return _raw;
    }

    template <typename T>
    operator T *() volatile {
        return reinterpret_cast<T *>(_raw);
    }

    template <typename T>
    operator T const *() const volatile {
        return reinterpret_cast<T const *>(_raw);
    }

    operator bool() const volatile {
        return _raw;
    }
};

template <typename T>
struct Ptr {
    u64 _raw;

    T *operator->() {
        return reinterpret_cast<T *>(_raw);
    }

    T const *operator->() const {
        return reinterpret_cast<T const *>(_raw);
    }

    T &operator*() {
        return *reinterpret_cast<T *>(_raw);
    }

    T const &operator*() const {
        return *reinterpret_cast<T const *>(_raw);
    }

    T &operator[](u64 index) {
        return reinterpret_cast<T *>(_raw)[index];
    }

    T const &operator[](u64 index) const {
        return reinterpret_cast<T const *>(_raw)[index];
    }

    operator bool() const {
        return _raw;
    }

    operator bool() const volatile {
        return _raw;
    }

    operator T *() {
        return reinterpret_cast<T *>(_raw);
    }

    operator T const *() const {
        return reinterpret_cast<T const *>(_raw);
    }

    operator T *() volatile {
        return reinterpret_cast<T *>(_raw);
    }
};

template <typename T>
struct List {
    u64 count;
    Ptr<Ptr<T>> items;

    T &operator[](u64 index) {
        return *items[index];
    }

    T const &operator[](u64 index) const {
        return *items[index];
    }

    operator bool() const {
        return count and items;
    }
};

struct File {
    uint64_t revision;
    Ptr<void> address;
    uint64_t size;
    Ptr<char> path;
    Ptr<char> cmdline;
    uint32_t mediaType;
    uint32_t unused;
    uint32_t tftpIp;
    uint32_t tftpPort;
    uint32_t partitionIndex;
    uint32_t mbrDiskId;
    Efi::Guid gptDiskGuid;
    Efi::Guid gptPartGuid;
    Efi::Guid partGuid;
};

template <typename Response>
struct Request {
    Id id;
    u64 revision;
    Ptr<Response> response;

    Response *operator->() {
        return (Response *)response;
    }

    Response const *operator->() const {
        return (Response *)response;
    }

    Response &operator*() {
        return *response;
    }

    Response const &operator*() const {
        return *response;
    }

    Response *operator->() volatile {
        return (Response *)response;
    }

    Response const *operator->() const volatile {
        return (Response *)response;
    }

    Response &operator*() volatile {
        return *response;
    }

    Response const &operator*() const volatile {
        return *response;
    }

    operator bool() const {
        return (bool)response;
    }

    operator bool() const volatile {
        return (bool)response;
    }
};

struct BootloaderInfo {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0xf55038d8e2a1202f,
        0x279426fcf5f59740,
    };

    static constexpr u64 REV = 0;

    struct Response : public Limine::Response {
        Ptr<char> name;
        Ptr<char> version;
    };

    using Request = Limine::Request<Response>;
};

struct StackSize {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x224ef0460a8e8926,
        0xe1cb0fc25f46ea3d,
    };

    static constexpr u64 REV = 0;

    using Response = Limine::Response;

    struct Request : public Limine::Request<Response> {
        u64 size;
    };
};

struct Hhdm {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x48dcf1cb8ad2b852,
        0x63984e959a98244b,
    };

    static constexpr u64 REV = 0;

    struct Response : public Limine::Response {
        u64 offset;
    };

    using Request = Limine::Request<Response>;
};

struct Framebuffer {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x9d5827dcd881dd75,
        0xa3148604f6fab11b,
    };

    static constexpr u64 REV = 0;

    struct Mode {
        u64 width;
        u64 height;
        u64 pitch;
        u16 bpp;
        u8 memoryModel;
        u8 redMaskSize;
        u8 redMaskShift;
        u8 greenMaskSize;
        u8 greenMaskShift;
        u8 blueMaskSize;
        u8 blueMaskShift;
    };

    struct _Framebuffer {
        Ptr<void> address;
        Mode mode;
        u8 _unused[7];
        u64 edidSize;
        Ptr<void> edid;
    };

    struct Response : public Limine::Response {
        List<_Framebuffer> framebuffer;
    };

    using Request = Limine::Request<Response>;
};

struct FiveLevelPaging {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x94469551da9b3192,
        0xebe5e86db7382888,
    };

    static constexpr u64 REV = 0;

    using Response = Limine::Response;

    using Request = Limine::Request<Response>;
};

struct Smp {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x95a67b819a1b857e,
        0xa0b61b723b6a73e0,
    };

    static constexpr u64 REV = 0;

    using Goto = void (*)(struct Info *);

#if defined(__x86_64__) || defined(__i386__)
    static auto const X2APIC = (1 << 0);

    struct Info {
        u32 processorId;
        u32 lapicId;
        u64 reserved;
        Goto gotoAddress;
        u64 extraArgument;
    };

    struct Response : public Limine::Response {
        u32 flags;
        u32 bspLapicId;
        List<Info> cpus;
    };
#elif defined(__aarch64__)
    struct Info {
        u32 processorId;
        u32 gicIfaceNo;
        u64 mpidr;
        u64 reserved;
        Goto gotoAddress;
        u64 extraArgument;
    };

    struct Response : public Limine::Response {
        u32 flags;
        u64 bspMpidr;
        List<Info> cpus;
    };
#else
#    error "Unsupported architecture"
#endif

    struct Request : public Limine::Request<Response> {
        u64 flags;
    };
};

struct Mmap {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x67cf3d9d378a806f,
        0xe304acdfc50c3c62,
    };

    static constexpr u64 REV = 0;

    static auto const USABLE = 0;
    static auto const RESERVED = 1;
    static auto const ACPI_RECLAIMABLE = 2;
    static auto const ACPI_NVS = 3;
    static auto const BAD_MEMORY = 4;
    static auto const BOOTLOADER_RECLAIMABLE = 5;
    static auto const KERNEL_AND_MODULES = 6;
    static auto const FRAMEBUFFER = 7;

    struct Entry {
        u64 base;
        u64 size;
        u64 type;
    };

    struct Response : public Limine::Response {
        List<Entry> entries;
    };

    using Request = Limine::Request<Response>;
};

struct EntryPoint {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x13d86c035a1cd3e1,
        0x2b0caa89d8f3026a,
    };

    static constexpr u64 REV = 0;

    using Response = Limine::Response;

    struct Request : public Limine::Request<Response> {
        Ptr<void> entry;
    };
};

struct KernelFile {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0xad97e90e83f1ed67,
        0x31eb5d1c5ff23b69,
    };

    static constexpr u64 REV = 0;

    struct Response : public Limine::Response {
        Ptr<File> file;
    };

    using Request = Limine::Request<Response>;
};

struct Module {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x3e7e279702be32af,
        0xca1c4f3bd1280cee,
    };

    static constexpr u64 REV = 1;

    static auto const REQUIRED = (1 << 0);

    struct Internal {
        Ptr<char> path;
        Ptr<char> cmdline;
        u64 flags;
    };

    struct Response : public Limine::Response {
        List<File> modules;
    };

    struct Request : public Limine::Request<Response> {
        List<Internal> internalModules;
    };
};

struct Rsdp {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0xc5e77b6b397e7b43,
        0x27637845accdcf3,
    };

    static constexpr u64 REV = 0;

    struct Response : public Limine::Response {
        Ptr<void> rsdp;
    };

    using Request = Limine::Request<Response>;
};

struct Smbios {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x9e9046f11e095391,
        0xaa4a520fefbde5ee,
    };

    struct Response : public Limine::Response {
        Ptr<void> entry32;
        Ptr<void> entry64;
    };

    using Request = Limine::Request<Response>;
};

struct EfiSystemTable {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x5ceba5163eaaf6d6,
        0x0a6981610cf65fcc,
    };

    static constexpr u64 REV = 0;

    struct Response : public Limine::Response {
        Ptr<void> address;
    };

    using Request = Limine::Request<Response>;
};

struct Boottime {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x502746e184c088aa,
        0xfbc5ec83e6327893,
    };

    static constexpr u64 REV = 0;

    struct Response : public Limine::Response {
        i64 bootTime;
    };

    using Request = Limine::Request<Response>;
};

struct KernelAddress {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0x71ba76863cc55f63,
        0xb2644a48c516a487,
    };

    static constexpr u64 REV = 0;

    struct Response : public Limine::Response {
        u64 paddr;
        u64 vaddr;
    };

    using Request = Limine::Request<Response>;
};

struct DeviceTreeBlob {
    static constexpr Id ID = {
        MAGIC0,
        MAGIC1,
        0xb40ddb48fb54bac7,
        0x545081493f81ffb7,
    };

    static constexpr u64 REV = 0;

    struct Response : public Limine::Response {
        Ptr<void> address;
    };

    using Request = Limine::Request<Response>;
};

template <typename R>
typename R::Request ask(typename R::Request request = {}) {
    request.id = R::ID;
    request.revision = R::REV;
    return request;
};

} // namespace Limine
