#pragma once

#define ERROR_ENUM(XX)                                        \
    XX(ERR_SUCESS)                                            \
    XX(ERR_BAD_FILE_DESCRIPTOR)                               \
    XX(ERR_NO_SUCH_FILE_OR_DIRECTORY)                         \
    XX(ERR_FILE_EXISTS)                                       \
    XX(ERR_OPERATION_NOT_PERMITTED)                           \
    XX(ERR_NO_SUCH_PROCESS)                                   \
    XX(ERR_INTERRUPTED_SYSTEM_CALL)                           \
    XX(ERR_INPUTOUTPUT_ERROR)                                 \
    XX(ERR_NO_SUCH_DEVICE_OR_ADDRESS)                         \
    XX(ERR_ARGUMENT_LIST_TOO_LONG)                            \
    XX(ERR_EXEC_FORMAT_ERROR)                                 \
    XX(ERR_NO_CHILD_PROCESSES)                                \
    XX(ERR_RESOURCE_TEMPORARILY_UNAVAILABLE)                  \
    XX(ERR_CANNOT_ALLOCATE_MEMORY)                            \
    XX(ERR_PERMISSION_DENIED)                                 \
    XX(ERR_BAD_ADDRESS)                                       \
    XX(ERR_BLOCK_DEVICE_REQUIRED)                             \
    XX(ERR_DEVICE_OR_RESOURCE_BUSY)                           \
    XX(ERR_INVALID_CROSS_DEVICE_LINK)                         \
    XX(ERR_NO_SUCH_DEVICE)                                    \
    XX(ERR_NOT_A_DIRECTORY)                                   \
    XX(ERR_IS_A_DIRECTORY)                                    \
    XX(ERR_INVALID_ARGUMENT)                                  \
    XX(ERR_TOO_MANY_OPEN_FILES_IN_SYSTEM)                     \
    XX(ERR_TOO_MANY_OPEN_FILES)                               \
    XX(ERR_INAPPROPRIATE_IOCTL_FOR_DEVICE)                    \
    XX(ERR_TEXT_FILE_BUSY)                                    \
    XX(ERR_FILE_TOO_LARGE)                                    \
    XX(ERR_NO_SPACE_LEFT_ON_DEVICE)                           \
    XX(ERR_ILLEGAL_SEEK)                                      \
    XX(ERR_READ_ONLY_FILE_SYSTEM)                             \
    XX(ERR_TOO_MANY_LINKS)                                    \
    XX(ERR_BROKEN_PIPE)                                       \
    XX(ERR_NUMERICAL_ARGUMENT_OUT_OF_DOMAIN)                  \
    XX(ERR_NUMERICAL_RESULT_OUT_OF_RANGE)                     \
    XX(ERR_RESOURCE_DEADLOCK_AVOIDED)                         \
    XX(ERR_FILE_NAME_TOO_LONG)                                \
    XX(ERR_NO_LOCKS_AVAILABLE)                                \
    XX(ERR_FUNCTION_NOT_IMPLEMENTED)                          \
    XX(ERR_DIRECTORY_NOT_EMPTY)                               \
    XX(ERR_TOO_MANY_LEVELS_OF_SYMBOLIC_LINKS)                 \
    XX(ERR_NO_MESSAGE_OF_DESIRED_TYPE)                        \
    XX(ERR_IDENTIFIER_REMOVED)                                \
    XX(ERR_CHANNEL_NUMBER_OUT_OF_RANGE)                       \
    XX(ERR_LINK_NUMBER_OUT_OF_RANGE)                          \
    XX(ERR_PROTOCOL_DRIVER_NOT_ATTACHED)                      \
    XX(ERR_NO_CSI_STRUCTURE_AVAILABLE)                        \
    XX(ERR_INVALID_EXCHANGE)                                  \
    XX(ERR_INVALID_REQUEST_DESCRIPTOR)                        \
    XX(ERR_EXCHANGE_FULL)                                     \
    XX(ERR_NO_ANODE)                                          \
    XX(ERR_INVALID_REQUEST_CODE)                              \
    XX(ERR_INVALID_SLOT)                                      \
    XX(ERR_BAD_FONT_FILE_FORMAT)                              \
    XX(ERR_DEVICE_NOT_A_STREAM)                               \
    XX(ERR_NO_DATA_AVAILABLE)                                 \
    XX(ERR_TIMER_EXPIRED)                                     \
    XX(ERR_OUT_OF_STREAMS_RESOURCES)                          \
    XX(ERR_MACHINE_IS_NOT_ON_THE_NETWORK)                     \
    XX(ERR_PACKAGE_NOT_INSTALLED)                             \
    XX(ERR_OBJECT_IS_REMOTE)                                  \
    XX(ERR_LINK_HAS_BEEN_SEVERED)                             \
    XX(ERR_ADVERTISE_ERROR)                                   \
    XX(ERR_SRMOUNT_ERROR)                                     \
    XX(ERR_COMMUNICATION_ERROR_ON_SEND)                       \
    XX(ERR_PROTOCOL_ERROR)                                    \
    XX(ERR_MULTIHOP_ATTEMPTED)                                \
    XX(ERR_RFS_SPECIFIC_ERROR)                                \
    XX(ERR_BAD_MESSAGE)                                       \
    XX(ERR_VALUE_TOO_LARGE_FOR_DEFINED_DATA_TYPE)             \
    XX(ERR_NAME_NOT_UNIQUE_ON_NETWORK)                        \
    XX(ERR_FILE_DESCRIPTOR_IN_BAD_STATE)                      \
    XX(ERR_REMOTE_ADDRESS_CHANGED)                            \
    XX(ERR_CAN_NOT_ACCESS_A_NEEDED_SHARED_LIBRARY)            \
    XX(ERR_ACCESSING_A_CORRUPTED_SHARED_LIBRARY)              \
    XX(ERR_ATTEMPTING_TO_LINK_IN_TOO_MANY_SHARED_LIBRARIES)   \
    XX(ERR_CANNOT_EXEC_A_SHARED_LIBRARY_DIRECTLY)             \
    XX(ERR_INVALID_OR_INCOMPLETE_MULTIBYTE_OR_WIDE_CHARACTER) \
    XX(ERR_INTERRUPTED_SYSTEM_CALL_SHOULD_BE_RESTARTED)       \
    XX(ERR_STREAMS_PIPE_ERROR)                                \
    XX(ERR_TOO_MANY_USERS)                                    \
    XX(ERR_SOCKET_OPERATION_ON_NON_SOCKET)                    \
    XX(ERR_DESTINATION_ADDRESS_REQUIRED)                      \
    XX(ERR_MESSAGE_TOO_LONG)                                  \
    XX(ERR_PROTOCOL_WRONG_TYPE_FOR_SOCKET)                    \
    XX(ERR_PROTOCOL_NOT_AVAILABLE)                            \
    XX(ERR_PROTOCOL_NOT_SUPPORTED)                            \
    XX(ERR_SOCKET_TYPE_NOT_SUPPORTED)                         \
    XX(ERR_PROTOCOL_FAMILY_NOT_SUPPORTED)                     \
    XX(ERR_ADDRESS_FAMILY_NOT_SUPPORTED_BY_PROTOCOL)          \
    XX(ERR_ADDRESS_ALREADY_IN_USE)                            \
    XX(ERR_CANNOT_ASSIGN_REQUESTED_ADDRESS)                   \
    XX(ERR_NETWORK_IS_DOWN)                                   \
    XX(ERR_NETWORK_IS_UNREACHABLE)                            \
    XX(ERR_NETWORK_DROPPED_CONNECTION_ON_RESET)               \
    XX(ERR_SOFTWARE_CAUSED_CONNECTION_ABORT)                  \
    XX(ERR_CONNECTION_RESET_BY_PEER)                          \
    XX(ERR_NO_BUFFER_SPACE_AVAILABLE)                         \
    XX(ERR_TRANSPORT_ENDPOINT_IS_ALREADY_CONNECTED)           \
    XX(ERR_TRANSPORT_ENDPOINT_IS_NOT_CONNECTED)               \
    XX(ERR_CANNOT_SEND_AFTER_TRANSPORT_ENDPOINT_SHUTDOWN)     \
    XX(ERR_TOO_MANY_REFERENCES_CANNOT_SPLICE)                 \
    XX(ERR_CONNECTION_TIMED_OUT)                              \
    XX(ERR_CONNECTION_REFUSED)                                \
    XX(ERR_HOST_IS_DOWN)                                      \
    XX(ERR_NO_ROUTE_TO_HOST)                                  \
    XX(ERR_OPERATION_ALREADY_IN_PROGRESS)                     \
    XX(ERR_OPERATION_NOW_IN_PROGRESS)                         \
    XX(ERR_STALE_FILE_HANDLE)                                 \
    XX(ERR_STRUCTURE_NEEDS_CLEANING)                          \
    XX(ERR_NOT_A_XENIX_NAMED_TYPE_FILE)                       \
    XX(ERR_NO_XENIX_SEMAPHORES_AVAILABLE)                     \
    XX(ERR_IS_A_NAMED_TYPE_FILE)                              \
    XX(ERR_REMOTE_IO_ERROR)                                   \
    XX(ERR_DISK_QUOTA_EXCEEDED)                               \
    XX(ERR_NO_MEDIUM_FOUND)                                   \
    XX(ERR_WRONG_MEDIUM_TYPE)                                 \
    XX(ERR_OPERATION_CANCELED)                                \
    XX(ERR_REQUIRED_KEY_NOT_AVAILABLE)                        \
    XX(ERR_KEY_HAS_EXPIRED)                                   \
    XX(ERR_KEY_HAS_BEEN_REVOKED)                              \
    XX(ERR_KEY_WAS_REJECTED_BY_SERVICE)                       \
    XX(ERR_OWNER_DIED)                                        \
    XX(ERR_STATE_NOT_RECOVERABLE)                             \
    XX(ERR_MEMORY_PAGE_HAS_HARDWARE_ERROR)                    \
    XX(ERR_OPERATION_NOT_SUPPORTED)

#define ERROR_ENUM_VALUE(__value) __value,

typedef enum
{
    ERROR_ENUM(ERROR_ENUM_VALUE)
} error_t;

#define RETURN_AND_SET_ERROR(__value, __return) \
    if ((__value) < 0)                          \
    {                                           \
        error_set(-(__value));                  \
        return (__return);                      \
    }                                           \
    else                                        \
    {                                           \
        return (__value);                       \
    }

const char *error_to_string(error_t error);
error_t error_get(void);
void error_set(error_t error);
void error_print(const char *message);
void if_error_throw_and_catch_fire(const char *message);