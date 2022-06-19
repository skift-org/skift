#pragma once

#include <karm-base/result.h>
#include <karm-base/std.h>
#include <karm-base/uuid.h>

namespace Efi {

using Handle = void *;

using Event = void *;

using Status = size_t;

struct Time {
    uint16_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
    uint8_t Pad1;
    uint32_t Nanosecond;
    int16_t TimeZone;
    uint8_t Daylight;
    uint8_t Pad2;
};

#define EFI_SUCCESS 0
#define EFI_ERROR 1ull << 63

#define FOREACH_ERROR(ERR)                      \
    ERR(LOAD_ERROR, EFI_ERROR | 0x1)            \
    ERR(INVALID_PARAMETER, EFI_ERROR | 0x2)     \
    ERR(UNSUPPORTED, EFI_ERROR | 0x3)           \
    ERR(BAD_BUFFER_SIZE, EFI_ERROR | 0x4)       \
    ERR(BUFFER_TOO_SMALL, EFI_ERROR | 0x5)      \
    ERR(NOT_READY, EFI_ERROR | 0x6)             \
    ERR(DEVICE_ERROR, EFI_ERROR | 0x7)          \
    ERR(WRITE_PROTECTED, EFI_ERROR | 0x8)       \
    ERR(OUT_OF_RESOURCES, EFI_ERROR | 0x9)      \
    ERR(VOLUME_CORRUPTED, EFI_ERROR | 0xa)      \
    ERR(VOLUME_FULL, EFI_ERROR | 0xb)           \
    ERR(NO_MEDIA, EFI_ERROR | 0xc)              \
    ERR(MEDIA_CHANGED, EFI_ERROR | 0xd)         \
    ERR(NOT_FOUND, EFI_ERROR | 0xe)             \
    ERR(ACCESS_DENIED, EFI_ERROR | 0xf)         \
    ERR(NO_RESPONSE, EFI_ERROR | 0x10)          \
    ERR(NO_MAPPING, EFI_ERROR | 0x11)           \
    ERR(TIMEOUT, EFI_ERROR | 0x12)              \
    ERR(NOT_STARTED, EFI_ERROR | 0x13)          \
    ERR(ALREADY_STARTED, EFI_ERROR | 0x14)      \
    ERR(ABORTED, EFI_ERROR | 0x15)              \
    ERR(ICMP_ERROR, EFI_ERROR | 0x16)           \
    ERR(TFTP_ERROR, EFI_ERROR | 0x17)           \
    ERR(PROTOCOL_ERROR, EFI_ERROR | 0x18)       \
    ERR(INCOMPATIBLE_VERSION, EFI_ERROR | 0x19) \
    ERR(SECURITY_VIOLATION, EFI_ERROR | 0x1a)   \
    ERR(CRC_ERROR, EFI_ERROR | 0x1b)            \
    ERR(END_OF_MEDIA, EFI_ERROR | 0x1c)         \
    ERR(END_OF_FILE, EFI_ERROR | 0x1f)          \
    ERR(INVALID_LANGUAGE, EFI_ERROR | 0x20)     \
    ERR(COMPROMISED_DATA, EFI_ERROR | 0x21)     \
    ERR(IP_ADDRESS_CONFLICT, EFI_ERROR | 0x22)  \
    ERR(HTTP_ERROR, EFI_ERROR | 0x23)

enum : size_t {
#define ERR(ERR, CODE) ERR_##ERR = CODE,
    FOREACH_ERROR(ERR)
#undef ERR
};

[[gnu::used]] static inline Error from_status(Status status) {
    if ((status & EFI_ERROR) == 0) {
        return OK;
    }

#define ERR(ERR, CODE)    \
    if (status == (CODE)) \
        return Error{Error::OTHER, #ERR};

    FOREACH_ERROR(ERR)

    return Error{Error::OTHER, "Unknown error"};

#undef ERR
}

template <typename... Args>
struct [[gnu::packed]] Method {
    Status (*func)(void *self, Args...);

    Error operator()(void *self, Args... args) {
        return from_status(func(self, args...));
    }
};

using DummyMethod = void *;

template <typename... Args>
struct [[gnu::packed]] Function {
    Status (*func)(Args...);

    Error operator()(Args... args) {
        return from_status(func(args...));
    }
};

using DummyFunction = void *;

static_assert(sizeof(Method<>) == sizeof(void *), "Method must be packed");

/* --- 4 System Table ------------------------------------------------------- */

struct TableHeader {
    uint64_t signature;
    uint32_t revision;
    uint32_t headerSize;
    uint32_t crc32;
    uint32_t reserved;
};

struct Table {
    TableHeader header;
};

/* --- 4.3 System Table ----------------------------------------------------- */

struct SimpleTextInputProtocol;
struct SimpleTextOutputProtocol;
struct RuntimeService;
struct BootService;

struct ConfigurationTable {
    Uuid vendorGuid;
    void *table;
};

struct SystemTable : public Table {
    uint16_t *firmwareVendor;
    uint32_t firmwareRevision;

    Handle *consoleInHandle;
    SimpleTextInputProtocol *conIn;

    Handle *consoleOutHandle;
    SimpleTextOutputProtocol *conOut;

    Handle *standardErrorHandle;
    SimpleTextOutputProtocol *stdErr;

    RuntimeService *runtime;
    BootService *boot;

    size_t nrConfigurationTables;
    ConfigurationTable *configurationTable;
};

/* --- 7 Boot Services ------------------------------------------------------ */

enum struct AllocateType : uint32_t {
    ANY_PAGES,
    MAX_ADDRESS,
    ADDRESS,
};

enum struct MemoryType : uint32_t {
    RESERVED_MEMORY_TYPE,
    LOADER_CODE,
    LOADER_DATA,
    BOOT_SERVICES_CODE,
    BOOT_SERVICES_DATA,
    RUNTIME_SERVICES_CODE,
    RUNTIME_SERVICES_DATA,
    CONVENTIONAL_MEMORY,
    UNUSABLE_MEMORY,
    ACPI_RECLAIM_MEMORY,
    ACPI_MEMORY_NVS,
    MEMORY_MAPPED_IO,
    MEMORY_MAPPED_IO_PORT_SPACE,
    PAL_CODE,
    PERSISTENT_MEMORY,

    MAX_MEMORY_TYPE,
};

struct MemoryDescriptor {
    MemoryType type;
    size_t physicalStart;
    size_t virtualStart;
    uint64_t numberOfPages;
    uint64_t attribute;
};

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

struct BootService : public Table {
    // Task Priority Services
    DummyFunction raiseTpl;
    DummyFunction lowerTpl;

    // Memory Services
    Function<AllocateType, MemoryType, size_t, size_t *> allocatePages;
    Function<size_t, size_t> freePages;
    Function<size_t *, MemoryDescriptor *, size_t *, size_t *, uint32_t *> getMemoryMap;
    Function<MemoryType, size_t, void **> allocatePool;
    Function<void *> freePool;

    // Event & Timer Services
    DummyFunction createEvent;
    DummyFunction setTimer;
    Function<size_t, Event *, size_t *> waitForEvent;
    DummyFunction signalEvent;
    DummyFunction closeEvent;
    DummyFunction checkEvent;

    // Protocol Handler Services
    DummyFunction installProtocolInterface;
    DummyFunction reinstallProtocolInterface;
    DummyFunction uninstallProtocolInterface;
    DummyFunction handleProtocol;
    DummyFunction _reserved;
    DummyFunction registerProtocolNotify;
    DummyFunction locateHandle;
    DummyFunction locateDevicePath;
    DummyFunction installConfigurationTable;

    // Image Services
    DummyFunction loadImage;
    DummyFunction startImage;
    DummyFunction exit;
    DummyFunction unloadImage;
    Function<Handle, size_t> exitBootServices;

    // Miscellaneous Services
    DummyFunction getNextMonotonicCount;
    DummyFunction stall;
    Function<size_t, uint64_t, size_t, uint16_t *> setWatchdogTimer;

    // DriverSupport Services
    DummyFunction connectController;
    DummyFunction disconnectController;

    // Open and Close Protocol Services
    Function<Handle, Uuid *, void **, Handle, Handle, uint32_t> openProtocol;
    Function<Handle, Uuid *, Handle, Handle> closeProtocol;
    DummyFunction openProtocolInformation;

    // Library Services
    DummyFunction protocolsPerHandle;
    DummyFunction locateHandleBuffer;
    Function<Uuid *, void *, void **> locateProtocol;
    DummyFunction installMultipleProtocolInterfaces;
    DummyFunction uninstallMultipleProtocolInterfaces;

    // 32-bit CRC Services
    DummyFunction calculateCrc32;

    // Miscellaneous Services
    DummyFunction copyMem;
    DummyFunction setMem;
    DummyFunction createEventEx;
};

/* --- 8 Runtime Services --------------------------------------------------- */

enum struct ResetType {
    RESET_COLD,
    RESET_WARM,
    RESET_SHUTDOWN,
    RESET_PLATFORM_SPECIFIC
};

struct RuntimeService : public Table {
    // Time Services
    DummyFunction getTime;
    DummyFunction setTime;
    DummyFunction getWakeupTime;
    DummyFunction setWakeupTime;

    // Virtual Memory Services
    DummyFunction setVirtualAddressMap;
    DummyFunction convertPointer;

    // Variable Services
    DummyFunction getVariable;
    DummyFunction getNextVariableName;
    DummyFunction setVariable;

    // Miscellaneous Services
    DummyFunction getNextMonotonicCount;
    Function<ResetType, Status, size_t, void *> resetSystem;

    // UEFI 2.0 Capsule Services
    DummyFunction updateCapsule;
    DummyFunction queryCapsuleCapabilities;

    // Miscellaneous UEFI 2.0 Services
    DummyFunction queryVariableInfo;
};

/* --- 9.1 Load Image Protocol ---------------------------------------------- */

struct DevicePathProtocol;

struct LoadedImageProtocol {
    static constexpr Uuid UUID = Uuid{0x5B1B31A1, 0x9562, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};

    uint32_t revision;
    Handle parentHandle;
    SystemTable *st;

    // Source location of the image
    Handle *deviceHandle;
    DevicePathProtocol *devicePath;
    void *reserved;

    // Image's load options
    uint32_t loadOptionsSize;
    void *loadOptions;

    // Location where image was loaded
    void *imageBase;
    uint64_t imageSize;
    MemoryType imageCodeType;
    MemoryType imageDataType;
    Function<Handle> unload;
};

/* --- 10 Device Path Protocol ---------------------------------------------- */

struct DevicePathProtocol {
    static constexpr Uuid UUID = Uuid{0x09576e91, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    uint8_t type;
    uint8_t subType;
    uint8_t length[2];
};

/* --- 12.3 Simple Text Input Protocol -------------------------------------- */

struct Key {
    uint16_t scanCode;
    uint16_t unicodeChar;
};

struct SimpleTextInputProtocol {
    static constexpr Uuid UUID = Uuid{0x387477c1, 0x69c7, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    Method<bool> reset;
    Method<Key *> readKeyStroke;
    Event waitForKey;
};

/* --- 12.4 Simple Text Output Protocol ------------------------------------- */

#define EFI_BLACK 0x00
#define EFI_BLUE 0x01
#define EFI_GREEN 0x02
#define EFI_CYAN 0x03
#define EFI_RED 0x04
#define EFI_MAGENTA 0x05
#define EFI_BROWN 0x06
#define EFI_LIGHTGRAY 0x07
#define EFI_BRIGHT 0x08

#define EFI_DARKGRAY (EFI_BLACK | EFI_BRIGHT)
#define EFI_LIGHTBLUE 0x09
#define EFI_LIGHTGREEN 0x0A
#define EFI_LIGHTCYAN 0x0B
#define EFI_LIGHTRED 0x0C
#define EFI_LIGHTMAGENTA 0x0D
#define EFI_YELLOW 0x0E
#define EFI_WHITE 0x0F

#define EFI_TEXT_ATTR(Foreground, Background) \
    ((Foreground) | ((Background) << 4))

struct SimpleTextOutputMode {
    uint32_t maxMode;
    uint32_t mode;
    uint32_t attribute;
    uint32_t cursorColumn;
    uint32_t cursorRow;
    bool cursorVisible;
};

struct SimpleTextOutputProtocol {
    static constexpr Uuid UUID = Uuid{0x387477c2, 0x69c7, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    Method<bool> reset;
    Method<uint16_t const *> outputString;
    Method<uint16_t const *> testString;
    Method<size_t, size_t *, size_t *> queryMode;
    Method<size_t> setMode;
    Method<size_t> setAttribute;
    Method<> clearScreen;
    Method<size_t, size_t> setCursorPosition;
    Method<bool> enableCursor;
    SimpleTextOutputMode *mode;
};

/* --- 12.9 Graphics Output Protocol ---------------------------------------- */

struct PixelBitmask {
    uint32_t redMask;
    uint32_t greenMask;
    uint32_t blueMask;
    uint32_t reservedMask;
};

enum struct PixelFormat : uint32_t {
    RED_GREEN_BLUE_RESERVED8_BIT_PER_COLOR,
    BLUE_GREEN_RED_RESERVED8_BIT_PER_COLOR,
    BIT_MASK,
    BLT_ONLY,
    FORMAT_MAX,
};

struct GraphicsOutputModeInformations {
    uint32_t version;
    uint32_t horizontalResolution;
    uint32_t verticalResolution;
    PixelFormat pixelFormat;
    PixelBitmask pixelInformation;
    uint32_t pixelsPerScanLine;
};

struct GraphicsOutputProtocolMode {
    uint32_t maxMode;
    uint32_t mode;
    GraphicsOutputModeInformations *info;
    size_t sizeOfInfo;
    size_t frameBufferBase;
    size_t frameBufferSize;
};

struct GraphicsOutputProtocol {
    Method<uint32_t, size_t *, GraphicsOutputProtocolMode **> queryMode;
    Method<size_t> setMode;
    DummyMethod blt;
    GraphicsOutputProtocolMode *mode;
};

/* --- 13.4 Simple File System Protocol ------------------------------------- */

struct FileProtocol;

struct SimpleFileSystemProtocol {
    static constexpr Uuid UUID = Uuid{0x0964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    uint64_t revision;
    Method<FileProtocol **> openVolume;
};

/* --- 13.5 File Protocol --------------------------------------------------- */

struct FileInfo {
    static constexpr Uuid UUID = Uuid{0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    uint64_t size;
    uint64_t fileSize;
    uint64_t physicalSize;
    Time createTime;
    Time lastAccessTime;
    Time modificationTime;

#define EFI_FILE_READ_ONLY 0x0000000000000001
#define EFI_FILE_HIDDEN 0x0000000000000002
#define EFI_FILE_SYSTEM 0x0000000000000004
#define EFI_FILE_RESERVED 0x0000000000000008
#define EFI_FILE_DIRECTORY 0x0000000000000010
#define EFI_FILE_ARCHIVE 0x0000000000000020
#define EFI_FILE_VALID_ATTR 0x0000000000000037
    uint64_t attribute;
    uint16_t fileName[];
};

struct FileIoToken {
    Event Event;
    Status Status;
    size_t BufferSize;
    void *Buffer;
};

struct FileProtocol {
    uint64_t revision;

#define EFI_FILE_MODE_READ 0x0000000000000001
#define EFI_FILE_MODE_WRITE 0x0000000000000002
#define EFI_FILE_MODE_CREATE 0x8000000000000000

#define EFI_FILE_READ_ONLY 0x0000000000000001
#define EFI_FILE_HIDDEN 0x0000000000000002
#define EFI_FILE_SYSTEM 0x0000000000000004
#define EFI_FILE_RESERVED 0x0000000000000008
#define EFI_FILE_DIRECTORY 0x0000000000000010
#define EFI_FILE_ARCHIVE 0x0000000000000020
#define EFI_FILE_VALID_ATTR 0x0000000000000037
    Method<FileProtocol **, uint16_t const *, uint64_t, uint64_t> open;
    Method<> close;
    Method<> del;
    Method<size_t *, void *> read;
    Method<size_t *, void const *> write;
    Method<uint64_t *> getPosition;
    Method<uint64_t> setPosition;
    Method<Uuid const *, size_t *, FileInfo *> getInfo;
    Method<Uuid const *, size_t *, FileInfo *> setInfo;
    Method<> flush;
};

}; // namespace Efi
