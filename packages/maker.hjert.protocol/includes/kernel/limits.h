#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* --- Limits for processes ------------------------------------------------- */

/* Maximum number of running processes. */
#define MAX_PROCESS 128

/* Size of a process name */
#define MAX_PROCESS_NAMESIZE 128 

/* Maximum number of opened files by a processes. */
#define MAX_PROCESS_OPENED_FILES 16 

/* Maximum number of pending messages for a processes. */
#define MAX_PROCESS_PENDING_MESSAGES 256

/* Maximum number of threads. */
#define MAX_THREAD 1024

/* Size of a thread name */
#define MAX_THREAD_NAMESIZE 128 

#define MAX_THREAD_STACKSIZE 16384

/* --- Limits for filesystem ------------------------------------------------ */

/* Maximum number of char in a path including \0. */
#define MAX_PATH_LENGHT 1024
#define MAX_FILENAME_LENGHT 128