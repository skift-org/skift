#pragma once

import Karm.Core;
import Karm.App;

#include "guid.h"

namespace Efi {

using Handle = void*;

using Event = void*;

using Status = usize;

struct Time {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u8 pad1;
    u32 nanosecond;
    i16 timeZone;
    u8 daylight;
    u8 pad2;
};

struct TimeCapabilities {
    u32 resolution;
    u32 accuracy;
    bool setsToZero;
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

enum : usize {

#define ERR(ERR, CODE) ERR_##ERR = CODE,
    FOREACH_ERROR(ERR)
#undef ERR
};

[[gnu::used]] inline Res<> fromStatus(Status status) {
    if ((status & EFI_ERROR) == 0) {
        return Ok();
    }

#define ERR(ERR, CODE)    \
    if (status == (CODE)) \
        return Error::other(#ERR);

    FOREACH_ERROR(ERR)

    return Error::other("unknown error");

#undef ERR
}

template <typename... Args>
struct [[gnu::packed]] Method {
    Status (*func)(void* self, Args...);

    Res<> operator()(void* self, Args... args) {
        return fromStatus(func(self, args...));
    }
};

using DummyMethod = void*;

template <typename... Args>
struct [[gnu::packed]] Function {
    Status (*func)(Args...);

    Res<> operator()(Args... args) {
        return fromStatus(func(args...));
    }
};

using DummyFunction = void*;

static_assert(sizeof(Method<>) == sizeof(void*), "Method must be packed");

// MARK: 4 System Table --------------------------------------------------------

struct TableHeader {
    u64 signature;
    u32 revision;
    u32 headerSize;
    u32 crc32;
    u32 reserved;
};

struct Table {
    TableHeader header;
};

// MARK: 4.3 System Table ------------------------------------------------------

struct SimpleTextInputProtocol;
struct SimpleTextOutputProtocol;
struct RuntimeService;
struct BootService;

struct ConfigurationTable {
    static constexpr Guid ACPI_TABLE_GUID = {0xeb9d2d30, 0x2d88, 0x11d3, {0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
    static constexpr Guid ACPI2_TABLE_GUID = {0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};

    Guid vendorGuid;
    void* table;
};

struct SystemTable : Table {
    u16* firmwareVendor;
    u32 firmwareRevision;

    Handle* consoleInHandle;
    SimpleTextInputProtocol* conIn;

    Handle* consoleOutHandle;
    SimpleTextOutputProtocol* conOut;

    Handle* standardErrorHandle;
    SimpleTextOutputProtocol* stdErr;

    RuntimeService* runtime;
    BootService* boot;

    usize nrConfigurationTables;
    ConfigurationTable* configurationTable;

    ConfigurationTable* lookupConfigurationTable(Guid const& guid) {
        for (usize i = 0; i < nrConfigurationTables; i++) {
            if (configurationTable[i].vendorGuid == guid) {
                return &configurationTable[i];
            }
        }

        return nullptr;
    }
};

// MARK: 7 Boot Services -------------------------------------------------------

enum struct AllocateType : u32 {
    ANY_PAGES,
    MAX_ADDRESS,
    ADDRESS,
};

enum struct MemoryType : u32 {
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
    usize physicalStart;
    usize virtualStart;
    u64 numberOfPages;
    u64 attribute;
};

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

struct BootService : Table {
    // Task Priority Services
    DummyFunction raiseTpl;
    DummyFunction lowerTpl;

    // Memory Services
    Function<AllocateType, MemoryType, usize, usize*> allocatePages;
    Function<usize, usize> freePages;
    Function<usize*, MemoryDescriptor*, usize*, usize*, u32*> getMemoryMap;
    Function<MemoryType, usize, void**> allocatePool;
    Function<void*> freePool;

    // Event & Timer Services
    DummyFunction createEvent;
    DummyFunction setTimer;
    Function<usize, Event*, usize*> waitForEvent;
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
    Function<Handle, usize> exitBootServices;

    // Miscellaneous Services
    DummyFunction getNextMonotonicCount;
    DummyFunction stall;
    Function<usize, u64, usize, u16*> setWatchdogTimer;

    // DriverSupport Services
    DummyFunction connectController;
    DummyFunction disconnectController;

    // Open and Close Protocol Services
    Function<Handle, Guid const*, void**, Handle, Handle, u32> openProtocol;
    Function<Handle, Guid const*, Handle, Handle> closeProtocol;
    DummyFunction openProtocolInformation;

    // Library Services
    DummyFunction protocolsPerHandle;
    DummyFunction locateHandleBuffer;
    Function<Guid const*, void*, void**> locateProtocol;
    DummyFunction installMultipleProtocolInterfaces;
    DummyFunction uninstallMultipleProtocolInterfaces;

    // 32-bit CRC Services
    DummyFunction calculateCrc32;

    // Miscellaneous Services
    DummyFunction copyMem;
    DummyFunction setMem;
    DummyFunction createEventEx;
};

// MARK: 8 Runtime Services ----------------------------------------------------

enum struct ResetType {
    RESET_COLD,
    RESET_WARM,
    RESET_SHUTDOWN,
    RESET_PLATFORM_SPECIFIC
};

struct RuntimeService : Table {
    // Time Services
    Function<Time*, TimeCapabilities*> getTime;
    Function<Time*> setTime;
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
    Function<ResetType, Status, usize, void*> resetSystem;

    // UEFI 2.0 Capsule Services
    DummyFunction updateCapsule;
    DummyFunction queryCapsuleCapabilities;

    // Miscellaneous UEFI 2.0 Services
    DummyFunction queryVariableInfo;
};

// MARK: 9.1 Load Image Protocol -----------------------------------------------

struct DevicePathProtocol;

struct LoadedImageProtocol {
    static constexpr Guid GUID = Guid{0x5B1B31A1, 0x9562, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};

    u32 revision;
    Handle parentHandle;
    SystemTable* st;

    // Source location of the image
    Handle* deviceHandle;
    DevicePathProtocol* devicePath;
    void* reserved;

    // Image's load options
    u32 loadOptionsSize;
    void* loadOptions;

    // Location where image was loaded
    void* imageBase;
    u64 imageSize;
    MemoryType imageCodeType;
    MemoryType imageDataType;
    Function<Handle> unload;
};

// MARK: 10 Device Path Protocol -----------------------------------------------

struct DevicePathProtocol {
    static constexpr Guid GUID = Guid{0x09576e91, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    u8 type;
    u8 subType;
    u8 length[2];
};

// MARK: 12.3 Simple Text Input Protocol ---------------------------------------

enum ScanCode : u16 {
    NONE = 0x00,
    UP = 0x01,
    DOWN = 0x02,
    RIGHT = 0x03,
    LEFT = 0x04,
    HOME = 0x05,
    END = 0x06,
    INSERT = 0x07,
    DELETE = 0x08,
    PGUP = 0x09,
    PGDOWN = 0x0a,
    F1 = 0x0b,
    F2 = 0x0c,
    F3 = 0x0d,
    F4 = 0x0e,
    F5 = 0x0f,
    F6 = 0x10,
    F7 = 0x11,
    F8 = 0x12,
    F9 = 0x13,
    F10 = 0x14,
    ESC = 0x17,
    F11 = 0x15,
    F12 = 0x16,
    F13 = 0x68,
    F14 = 0x69,
    F15 = 0x6A,
    F16 = 0x6B,
    F17 = 0x6C,
    F18 = 0x6D,
    F19 = 0x6E,
    F20 = 0x6F,
    F21 = 0x70,
    F22 = 0x71,
    F23 = 0x72,
    F24 = 0x73,
    MUTE = 0x7F,
    VOLUP = 0x80,
    VOLDOWN = 0x81,
    BRIGHTNESSUP = 0x100,
    BRIGHTNESSDOWN = 0x101,
    SUSPEND = 0x102,
    HIBERNATE = 0x103,
    TOGGLE_DISPLAY = 0x104,
    RECOVERY = 0x105,
    EJECT = 0x106,
};

struct Key {
    ScanCode scanCode;
    u16 unicodeChar;

    App::KeyboardEvent toKeyEvent() const {
        App::Key key;

        switch (scanCode) {
        case NONE:
            switch (unicodeChar) {
            case u'\r':
                key = App::Key::ENTER;
                break;
            default:
                key = App::Key::INVALID;
                break;
            }
            break;
        case UP:
            key = App::Key::UP;
            break;
        case DOWN:
            key = App::Key::DOWN;
            break;

        case RIGHT:
            key = App::Key::RIGHT;
            break;

        case LEFT:
            key = App::Key::LEFT;
            break;

        case HOME:
        case END:
        case INSERT:
        case DELETE:
        case PGUP:
        case PGDOWN:
        case F1:
        case F2:
        case F3:
        case F4:
        case F5:
        case F6:
        case F7:
        case F8:
        case F9:
        case F10:
        case ESC:
        case F11:
        case F12:
        case F13:
        case F14:
        case F15:
        case F16:
        case F17:
        case F18:
        case F19:
        case F20:
        case F21:
        case F22:
        case F23:
        case F24:
        case MUTE:
        case VOLUP:
        case VOLDOWN:
        case BRIGHTNESSUP:
        case BRIGHTNESSDOWN:
        case SUSPEND:
        case HIBERNATE:
        case TOGGLE_DISPLAY:
        case RECOVERY:
        case EJECT:
        default:
            key = App::Key::INVALID;
            break;
        }

        return {
            .type = App::KeyboardEvent::PRESS,
            .key = key,
            .code = key,
            .mods = {}
        };
    }
};

struct SimpleTextInputProtocol {
    static constexpr Guid GUID = Guid{0x387477c1, 0x69c7, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    Method<bool> reset;
    Method<Key*> readKeyStroke;
    Event waitForKey;
};

// MARK: 12.4 Simple Text Output Protocol --------------------------------------

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
    u32 maxMode;
    u32 mode;
    u32 attribute;
    u32 cursorColumn;
    u32 cursorRow;
    bool cursorVisible;
};

struct SimpleTextOutputProtocol {
    static constexpr Guid GUID = Guid{0x387477c2, 0x69c7, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    Method<bool> reset;
    Method<u16 const*> outputString;
    Method<u16 const*> testString;
    Method<usize, usize*, usize*> queryMode;
    Method<usize> setMode;
    Method<usize> setAttribute;
    Method<> clearScreen;
    Method<usize, usize> setCursorPosition;
    Method<bool> enableCursor;
    SimpleTextOutputMode* mode;
};

// MARK: 12.9 Graphics Output Protocol -----------------------------------------

struct PixelBitmask {
    u32 redMask;
    u32 greenMask;
    u32 blueMask;
    u32 reservedMask;
};

enum struct PixelFormat : u32 {
    RED_GREEN_BLUE_RESERVED8_BIT_PER_COLOR,
    BLUE_GREEN_RED_RESERVED8_BIT_PER_COLOR,
    BIT_MASK,
    BLT_ONLY,
    FORMAT_MAX,
};

struct GraphicsOutputModeInformations {
    u32 version;
    u32 horizontalResolution;
    u32 verticalResolution;
    PixelFormat pixelFormat;
    PixelBitmask pixelInformation;
    u32 pixelsPerScanLine;
};

struct GraphicsOutputProtocolMode {
    u32 maxMode;
    u32 mode;
    GraphicsOutputModeInformations* info;
    usize sizeOfInfo;
    usize frameBufferBase;
    usize frameBufferSize;
};

struct GraphicsOutputProtocol {
    static constexpr Guid GUID = Guid{0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}};

    Method<u32, usize*, GraphicsOutputProtocolMode**> queryMode;
    Method<usize> setMode;
    DummyMethod blt;
    GraphicsOutputProtocolMode* mode;
};

// MARK: 13.4 Simple File System Protocol --------------------------------------

struct FileProtocol;

struct SimpleFileSystemProtocol {
    static constexpr Guid GUID = Guid{0x0964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    u64 revision;
    Method<FileProtocol**> openVolume;
};

// MARK: 13.5 File Protocol ----------------------------------------------------

struct FileInfo {
    static constexpr Guid GUID = Guid{0x09576e92, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};

    u64 size;
    u64 fileSize;
    u64 physicalSize;
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
    u64 attribute;
    u16 fileName[];
};

struct FileIoToken {
    Event Event;
    Status Status;
    usize BufferSize;
    void* Buffer;
};

struct FileProtocol {
    u64 revision;

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
    Method<FileProtocol**, u16 const*, u64, u64> open;
    Method<> close;
    Method<> del;
    Method<usize*, void*> read;
    Method<usize*, void const*> write;
    Method<u64*> getPosition;
    Method<u64> setPosition;
    Method<Guid const*, usize*, FileInfo*> getInfo;
    Method<Guid const*, usize*, FileInfo*> setInfo;
    Method<> flush;
};

} // namespace Efi
