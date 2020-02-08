#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Syscalls.h>

#include <libsystem/runtime.h>

int task_do_syscall(Syscall syscall, int arg0, int arg1, int arg2, int arg3, int arg4);