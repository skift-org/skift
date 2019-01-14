#pragma once

/* --- Limits for processes ------------------------------------------------- */

/* Maximum number of running processes. */
#define MAX_PROCESS 1024 

/* Maximum number of running threads per processes. */
#define MAX_PROCESS_THREAD 16 

/* Maximum number of opened files by a processes. */
#define MAX_PROCESS_OPENED_FILES 16 

/* Maximum number of pending messages for a processes. */
#define MAX_PROCESS_PENDING_MESSAGES 256

/* --- Limits for filesystem ------------------------------------------------ */

/* Maximum number of char in a path including \0. */
#define MAX_PATH_LENGHT 1024
#define MAX_FILENAME_LENGHT 128