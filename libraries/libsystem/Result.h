#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#define RESULT_ENUM(__ENTRY, __ENTRY_WITH_VALUE)                   \
    __ENTRY_WITH_VALUE(SUCCESS, 0)                                 \
    __ENTRY(TIMEOUT)                                               \
    __ENTRY(ERR_INBOX_FULL)                                        \
    __ENTRY(ERR_BAD_FILE_DESCRIPTOR)                               \
    __ENTRY(ERR_NO_SUCH_FILE_OR_DIRECTORY)                         \
    __ENTRY(ERR_FILE_EXISTS)                                       \
    __ENTRY(ERR_OPERATION_NOT_PERMITTED)                           \
    __ENTRY(ERR_NO_SUCH_PROCESS)                                   \
    __ENTRY(ERR_INTERRUPTED_SYSTEM_CALL)                           \
    __ENTRY(ERR_INPUTOUTPUT_ERROR)                                 \
    __ENTRY(ERR_NO_SUCH_DEVICE_OR_ADDRESS)                         \
    __ENTRY(ERR_ARGUMENT_LIST_TOO_LONG)                            \
    __ENTRY(ERR_EXEC_FORMAT_ERROR)                                 \
    __ENTRY(ERR_NO_CHILD_PROCESSES)                                \
    __ENTRY(ERR_RESOURCE_TEMPORARILY_UNAVAILABLE)                  \
    __ENTRY(ERR_CANNOT_ALLOCATE_MEMORY)                            \
    __ENTRY(ERR_PERMISSION_DENIED)                                 \
    __ENTRY(ERR_BAD_ADDRESS)                                       \
    __ENTRY(ERR_BLOCK_DEVICE_REQUIRED)                             \
    __ENTRY(ERR_DEVICE_OR_RESOURCE_BUSY)                           \
    __ENTRY(ERR_INVALID_CROSS_DEVICE_LINK)                         \
    __ENTRY(ERR_NO_SUCH_DEVICE)                                    \
    __ENTRY(ERR_NOT_A_DIRECTORY)                                   \
    __ENTRY(ERR_NOT_A_STREAM)                                      \
    __ENTRY(ERR_NOT_A_SOCKET)                                      \
    __ENTRY(ERR_IS_A_DIRECTORY)                                    \
    __ENTRY(ERR_INVALID_ARGUMENT)                                  \
    __ENTRY(ERR_TOO_MANY_OPEN_FILES_IN_SYSTEM)                     \
    __ENTRY(ERR_TOO_MANY_OPEN_FILES)                               \
    __ENTRY(ERR_INAPPROPRIATE_CALL_FOR_DEVICE)                     \
    __ENTRY(ERR_TEXT_FILE_BUSY)                                    \
    __ENTRY(ERR_FILE_TOO_LARGE)                                    \
    __ENTRY(ERR_NO_SPACE_LEFT_ON_DEVICE)                           \
    __ENTRY(ERR_ILLEGAL_SEEK)                                      \
    __ENTRY(ERR_READ_ONLY_FILE_SYSTEM)                             \
    __ENTRY(ERR_READ_ONLY_STREAM)                                  \
    __ENTRY(ERR_WRITE_ONLY_STREAM)                                 \
    __ENTRY(ERR_NOT_READABLE)                                      \
    __ENTRY(ERR_NOT_WRITABLE)                                      \
    __ENTRY(ERR_TOO_MANY_LINKS)                                    \
    __ENTRY(ERR_BROKEN_PIPE)                                       \
    __ENTRY(ERR_NUMERICAL_ARGUMENT_OUT_OF_DOMAIN)                  \
    __ENTRY(ERR_NUMERICAL_RESULT_OUT_OF_RANGE)                     \
    __ENTRY(ERR_RESOURCE_DEADLOCK_AVOIDED)                         \
    __ENTRY(ERR_FILE_NAME_TOO_LONG)                                \
    __ENTRY(ERR_NO_LOCKS_AVAILABLE)                                \
    __ENTRY(ERR_FUNCTION_NOT_IMPLEMENTED)                          \
    __ENTRY(ERR_DIRECTORY_NOT_EMPTY)                               \
    __ENTRY(ERR_TOO_MANY_LEVELS_OF_SYMBOLIC_LINKS)                 \
    __ENTRY(ERR_NO_MESSAGE_OF_DESIRED_TYPE)                        \
    __ENTRY(ERR_IDENTIFIER_REMOVED)                                \
    __ENTRY(ERR_CHANNEL_NUMBER_OUT_OF_RANGE)                       \
    __ENTRY(ERR_LINK_NUMBER_OUT_OF_RANGE)                          \
    __ENTRY(ERR_PROTOCOL_DRIVER_NOT_ATTACHED)                      \
    __ENTRY(ERR_NO_CSI_STRUCTURE_AVAILABLE)                        \
    __ENTRY(ERR_INVALID_EXCHANGE)                                  \
    __ENTRY(ERR_INVALID_REQUEST_DESCRIPTOR)                        \
    __ENTRY(ERR_EXCHANGE_FULL)                                     \
    __ENTRY(ERR_NO_ANODE)                                          \
    __ENTRY(ERR_INVALID_REQUEST_CODE)                              \
    __ENTRY(ERR_INVALID_SLOT)                                      \
    __ENTRY(ERR_BAD_FONT_FILE_FORMAT)                              \
    __ENTRY(ERR_BAD_IMAGE_FILE_FORMAT)                             \
    __ENTRY(ERR_DEVICE_NOT_A_STREAM)                               \
    __ENTRY(ERR_NO_DATA_AVAILABLE)                                 \
    __ENTRY(ERR_TIMER_EXPIRED)                                     \
    __ENTRY(ERR_OUT_OF_STREAMS_RESOURCES)                          \
    __ENTRY(ERR_MACHINE_IS_NOT_ON_THE_NETWORK)                     \
    __ENTRY(ERR_PACKAGE_NOT_INSTALLED)                             \
    __ENTRY(ERR_OBJECT_IS_REMOTE)                                  \
    __ENTRY(ERR_LINK_HAS_BEEN_SEVERED)                             \
    __ENTRY(ERR_ADVERTISE_ERROR)                                   \
    __ENTRY(ERR_SRMOUNT_ERROR)                                     \
    __ENTRY(ERR_COMMUNICATION_ERROR_ON_SEND)                       \
    __ENTRY(ERR_PROTOCOL_ERROR)                                    \
    __ENTRY(ERR_MULTIHOP_ATTEMPTED)                                \
    __ENTRY(ERR_RFS_SPECIFIC_ERROR)                                \
    __ENTRY(ERR_BAD_MESSAGE)                                       \
    __ENTRY(ERR_NO_MESSAGE)                                        \
    __ENTRY(ERR_VALUE_TOO_LARGE_FOR_DEFINED_DATA_TYPE)             \
    __ENTRY(ERR_NAME_NOT_UNIQUE_ON_NETWORK)                        \
    __ENTRY(ERR_FILE_DESCRIPTOR_IN_BAD_STATE)                      \
    __ENTRY(ERR_REMOTE_ADDRESS_CHANGED)                            \
    __ENTRY(ERR_CAN_NOT_ACCESS_A_NEEDED_SHARED_LIBRARY)            \
    __ENTRY(ERR_ACCESSING_A_CORRUPTED_SHARED_LIBRARY)              \
    __ENTRY(ERR_ATTEMPTING_TO_LINK_IN_TOO_MANY_SHARED_LIBRARIES)   \
    __ENTRY(ERR_CANNOT_EXEC_A_SHARED_LIBRARY_DIRECTLY)             \
    __ENTRY(ERR_INVALID_OR_INCOMPLETE_MULTIBYTE_OR_WIDE_CHARACTER) \
    __ENTRY(ERR_INTERRUPTED_SYSTEM_CALL_SHOULD_BE_RESTARTED)       \
    __ENTRY(ERR_STREAMS_PIPE_ERROR)                                \
    __ENTRY(ERR_TOO_MANY_USERS)                                    \
    __ENTRY(ERR_SOCKET_OPERATION_ON_NON_SOCKET)                    \
    __ENTRY(ERR_DESTINATION_ADDRESS_REQUIRED)                      \
    __ENTRY(ERR_MESSAGE_TOO_LONG)                                  \
    __ENTRY(ERR_PROTOCOL_WRONG_TYPE_FOR_SOCKET)                    \
    __ENTRY(ERR_PROTOCOL_NOT_AVAILABLE)                            \
    __ENTRY(ERR_PROTOCOL_NOT_SUPPORTED)                            \
    __ENTRY(ERR_SOCKET_TYPE_NOT_SUPPORTED)                         \
    __ENTRY(ERR_PROTOCOL_FAMILY_NOT_SUPPORTED)                     \
    __ENTRY(ERR_ADDRESS_FAMILY_NOT_SUPPORTED_BY_PROTOCOL)          \
    __ENTRY(ERR_ADDRESS_ALREADY_IN_USE)                            \
    __ENTRY(ERR_CANNOT_ASSIGN_REQUESTED_ADDRESS)                   \
    __ENTRY(ERR_NETWORK_IS_DOWN)                                   \
    __ENTRY(ERR_NETWORK_IS_UNREACHABLE)                            \
    __ENTRY(ERR_NETWORK_DROPPED_CONNECTION_ON_RESET)               \
    __ENTRY(ERR_SOFTWARE_CAUSED_CONNECTION_ABORT)                  \
    __ENTRY(ERR_CONNECTION_RESET_BY_PEER)                          \
    __ENTRY(ERR_NO_BUFFER_SPACE_AVAILABLE)                         \
    __ENTRY(ERR_TRANSPORT_ENDPOINT_IS_ALREADY_CONNECTED)           \
    __ENTRY(ERR_TRANSPORT_ENDPOINT_IS_NOT_CONNECTED)               \
    __ENTRY(ERR_CANNOT_SEND_AFTER_TRANSPORT_ENDPOINT_SHUTDOWN)     \
    __ENTRY(ERR_TOO_MANY_REFERENCES_CANNOT_SPLICE)                 \
    __ENTRY(ERR_CONNECTION_TIMED_OUT)                              \
    __ENTRY(ERR_CONNECTION_REFUSED)                                \
    __ENTRY(ERR_HOST_IS_DOWN)                                      \
    __ENTRY(ERR_NO_ROUTE_TO_HOST)                                  \
    __ENTRY(ERR_OPERATION_ALREADY_IN_PROGRESS)                     \
    __ENTRY(ERR_OPERATION_NOW_IN_PROGRESS)                         \
    __ENTRY(ERR_STALE_FILE_HANDLE)                                 \
    __ENTRY(ERR_STRUCTURE_NEEDS_CLEANING)                          \
    __ENTRY(ERR_NOT_A_XENIX_NAMED_TYPE_FILE)                       \
    __ENTRY(ERR_NO_XENIX_SEMAPHORES_AVAILABLE)                     \
    __ENTRY(ERR_IS_A_NAMED_TYPE_FILE)                              \
    __ENTRY(ERR_REMOTE_IO_ERROR)                                   \
    __ENTRY(ERR_DISK_QUOTA_EXCEEDED)                               \
    __ENTRY(ERR_NO_MEDIUM_FOUND)                                   \
    __ENTRY(ERR_WRONG_MEDIUM_TYPE)                                 \
    __ENTRY(ERR_OPERATION_CANCELED)                                \
    __ENTRY(ERR_REQUIRED_KEY_NOT_AVAILABLE)                        \
    __ENTRY(ERR_KEY_HAS_EXPIRED)                                   \
    __ENTRY(ERR_KEY_HAS_BEEN_REVOKED)                              \
    __ENTRY(ERR_KEY_WAS_REJECTED_BY_SERVICE)                       \
    __ENTRY(ERR_OWNER_DIED)                                        \
    __ENTRY(ERR_STATE_NOT_RECOVERABLE)                             \
    __ENTRY(ERR_MEMORY_PAGE_HAS_HARDWARE_ERROR)                    \
    __ENTRY(ERR_OPERATION_NOT_SUPPORTED)                           \
    __ENTRY(ERR_REQUEST_TIMEOUT)

#define RESULT_ENUM_ENTRY(__entry) __entry,
#define RESULT_ENUM_ENTRY_WITH_VALUE(__entry, __value) __entry = __value,

typedef enum
{
    RESULT_ENUM(RESULT_ENUM_ENTRY, RESULT_ENUM_ENTRY_WITH_VALUE)
} Result;

#define RETURN_AND_SET_ERROR(__value, __good_return, __on_error_return) \
    if ((__value) < 0)                                                  \
    {                                                                   \
        error_set(-(__value));                                          \
        return (__on_error_return);                                     \
    }                                                                   \
    else                                                                \
    {                                                                   \
        return (__good_return);                                         \
    }

const char *result_to_string(Result error);

Result error_get(void);

void error_set(Result error);

void error_print(const char *message);

#define error_log(__message) logger_log("%s: %s\n", __message, result_to_string(error_value));