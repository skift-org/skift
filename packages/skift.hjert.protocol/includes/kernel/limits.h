#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
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

#define MAX_PROCESS_ARGV 32

/* Maximum number of threads. */
#define MAX_THREAD 512

/* Size of a thread name */
#define MAX_THREAD_NAMESIZE 128 

#define MAX_THREAD_STACKSIZE 16384