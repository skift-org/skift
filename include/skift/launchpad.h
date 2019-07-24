#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

// Create a new launchpad process
int launchpad_create(const char *name);

// Abort the lauchpad process
int launchpad_abort(int launchpad);

// Start the process and destroy the launchpad
int launchpad_ready(int launchpad);

// Copy some memory from a process to the launchpad
int launchpad_copyout(int launchpad, uint dest, uint src, uint count);

// Get the entry point of the launchpad
uint launchpad_get_entry(int launchpad);

// Set the entry point of the launchpad
int launchpad_set_entry(int launchpad, uint entry);

// Load a elf file in the lauchpad memory
int launchpad_load_elf(int launchpad, const char *elf_path);

// Dup a file descriptor from a process and pass it to the launch pad
int launchpad_dup_fd(int launchpad, int fd, int target_fd);

// Pass a file descriptor from a process to the launchpad.
int launchpad_pass_fd(int launchpad, int fd, int target_fd);

// Create a pipe between the process and the launch pad.
int launchpad_pass_fd(int launchpad, int fd, int target_fd);