#pragma once

#define RESULT_ENUM(__ENTRY, __ENTRY_WITH_VALUE) \
    __ENTRY_WITH_VALUE(SUCCESS, 0)               \
    __ENTRY(TIMEOUT)                             \
    __ENTRY(ERR_BAD_ADDRESS)                     \
    __ENTRY(ERR_BAD_FILE_DESCRIPTOR)             \
    __ENTRY(ERR_BAD_FONT_FILE_FORMAT)            \
    __ENTRY(ERR_BAD_IMAGE_FILE_FORMAT)           \
    __ENTRY(ERR_CANNOT_ALLOCATE_MEMORY)          \
    __ENTRY(ERR_CONNECTION_REFUSED)              \
    __ENTRY(ERR_EXEC_FORMAT_ERROR)               \
    __ENTRY(ERR_FILE_EXISTS)                     \
    __ENTRY(ERR_FUNCTION_NOT_IMPLEMENTED)        \
    __ENTRY(ERR_INAPPROPRIATE_CALL_FOR_DEVICE)   \
    __ENTRY(ERR_INVALID_ARGUMENT)                \
    __ENTRY(ERR_IS_A_DIRECTORY)                  \
    __ENTRY(ERR_MEMORY_NOT_ALIGNED)              \
    __ENTRY(ERR_NO_SUCH_DEVICE)                  \
    __ENTRY(ERR_NO_SUCH_FILE_OR_DIRECTORY)       \
    __ENTRY(ERR_NO_SUCH_TASK)                    \
    __ENTRY(ERR_NOT_A_DIRECTORY)                 \
    __ENTRY(ERR_NOT_A_SOCKET)                    \
    __ENTRY(ERR_NOT_A_STREAM)                    \
    __ENTRY(ERR_NOT_READABLE)                    \
    __ENTRY(ERR_NOT_WRITABLE)                    \
    __ENTRY(ERR_OPERATION_NOT_SUPPORTED)         \
    __ENTRY(ERR_OUT_OF_MEMORY)                   \
    __ENTRY(ERR_READ_ONLY_STREAM)                \
    __ENTRY(ERR_SOCKET_OPERATION_ON_NON_SOCKET)  \
    __ENTRY(ERR_STREAM_CLOSED)                   \
    __ENTRY(ERR_TOO_MANY_OPEN_FILES)             \
    __ENTRY(ERR_WRITE_ONLY_STREAM)

#define RESULT_ENUM_ENTRY(__entry) __entry,
#define RESULT_ENUM_ENTRY_WITH_VALUE(__entry, __value) __entry = __value,

typedef enum
{
    RESULT_ENUM(RESULT_ENUM_ENTRY, RESULT_ENUM_ENTRY_WITH_VALUE)
        __RESULT_COUNT,
} Result;

const char *result_to_string(Result error);

Result error_get(void);

void error_set(Result error);

void error_print(const char *message);

#define error_log(__message) logger_log("%s: %s\n", __message, result_to_string(error_value));
