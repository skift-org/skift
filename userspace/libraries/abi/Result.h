#pragma once

#define RESULT_ENUM(__ENTRY)                                                      \
    __ENTRY(SUCCESS, "Success")                                                   \
    __ENTRY(TIMEOUT, "Timed out")                                                 \
    __ENTRY(INTERRUPTED, "Interrupted")                                           \
    __ENTRY(ERR_BAD_ADDRESS, "Bad address")                                       \
    __ENTRY(ERR_BAD_HANDLE, "Bad file descriptor")                                \
    __ENTRY(ERR_BAD_FONT_FILE_FORMAT, "Bad font file format")                     \
    __ENTRY(ERR_BAD_IMAGE_FILE_FORMAT, "Bad image file format")                   \
    __ENTRY(ERR_CANNOT_ALLOCATE_MEMORY, "Cannot allocate memory")                 \
    __ENTRY(ERR_CONNECTION_REFUSED, "Connection refused")                         \
    __ENTRY(ERR_EXEC_FORMAT_ERROR, "Exec format error")                           \
    __ENTRY(ERR_FILE_EXISTS, "File exists")                                       \
    __ENTRY(ERR_NOT_IMPLEMENTED, "Function not implemented")                      \
    __ENTRY(ERR_INAPPROPRIATE_CALL_FOR_DEVICE, "Inappropriate call for device")   \
    __ENTRY(ERR_INVALID_ARGUMENT, "Invalid argument")                             \
    __ENTRY(ERR_INVALID_DATA, "Invalid data")                                     \
    __ENTRY(ERR_IS_A_DIRECTORY, "File is a directory")                            \
    __ENTRY(ERR_MEMORY_NOT_ALIGNED, "Memory not aligned")                         \
    __ENTRY(ERR_NO_SUCH_DEVICE, "No such device")                                 \
    __ENTRY(ERR_NO_SUCH_FILE_OR_DIRECTORY, "No such file or directory")           \
    __ENTRY(ERR_NO_SUCH_TASK, "No such task")                                     \
    __ENTRY(ERR_TASK_NOT_WAITABLE, "Task not waitable")                           \
    __ENTRY(ERR_NOT_A_DIRECTORY, "File is not a directory")                       \
    __ENTRY(ERR_NOT_A_SOCKET, "Not a socket")                                     \
    __ENTRY(ERR_NOT_A_STREAM, "Not a stream")                                     \
    __ENTRY(ERR_NOT_READABLE, "Not readable")                                     \
    __ENTRY(ERR_NOT_WRITABLE, "Not writable")                                     \
    __ENTRY(ERR_OPERATION_NOT_SUPPORTED, "Unsupported operation")                 \
    __ENTRY(ERR_OUT_OF_MEMORY, "Out of memory")                                   \
    __ENTRY(ERR_READ_ONLY_STREAM, "Read only stream")                             \
    __ENTRY(ERR_SOCKET_OPERATION_ON_NON_SOCKET, "Socket operation on non socket") \
    __ENTRY(ERR_STREAM_CLOSED, "Stream closed")                                   \
    __ENTRY(ERR_TOO_MANY_HANDLE, "Too many files open")                           \
    __ENTRY(ERR_WRITE_ONLY_STREAM, "Write only stream")                           \
    __ENTRY(ERR_DIRECTORY_NOT_EMPTY, "Directory not empty")                       \
    __ENTRY(ERR_EXTENSION, "Unrecognized file extension")                         \
    __ENTRY(ERR_ACCESS_DENIED, "Access denied")                                   \
    __ENTRY(ERR_UNKNOWN, "Unknown failure")

enum HjResult
{
#define RESULT_ENUM_ENTRY(__name, __description) __name,
    RESULT_ENUM(RESULT_ENUM_ENTRY)
#undef RESULT_ENUM_ENTRY
        __RESULT_COUNT
};

#define result_is_error(__result) ((__result) != SUCCESS && (__result) != TIMEOUT)

static inline const char *result_to_string(HjResult error)
{
#define RESULT_ENUM_ENTRY_STRING(__entry, __description) #__entry,
    const char *RESULT_NAMES[] = {RESULT_ENUM(RESULT_ENUM_ENTRY_STRING)};
#undef RESULT_ENUM_ENTRY

    if (error < __RESULT_COUNT && error >= 0)
    {
        return RESULT_NAMES[error];
    }
    else
    {
        return "INVALID_RESULT_CODE";
    }
}

static inline const char *get_result_description(HjResult result)
{
    const char *RESULT_DESCRIPTIONS[] = {
#define RESULT_ENUM_ENTRY(__name, __description) __description,
        RESULT_ENUM(RESULT_ENUM_ENTRY)
#undef RESULT_ENUM_ENTRY
    };

    if (result < __RESULT_COUNT && result >= 0)
    {
        return RESULT_DESCRIPTIONS[result];
    }
    else
    {
        return "INVALID_RESULT_CODE";
    }
}
