#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef struct 
{
    int handle;
    bool go;
} launchpad_t;

// Create a new launchpad process
launchpad_t* launchpad(const char *name);

// Start the process and destroy the launchpad
int launchpad_go(launchpad_t* this);

// Copy some memory from a process to the launchpad
int launchpad_copy_out(launchpad_t* this, uint dest, uint src, uint count);

// Get the entry point of the launchpad
uint launchpad_get_entry(launchpad_t* this);

// Set the entry point of the launchpad
int launchpad_set_entry(launchpad_t* this, uint entry);

// Load a elf file in the lauchpad memory
int launchpad_load_elf(launchpad_t* this, const char *elf_path);

// Dup a file descriptor from a process and pass it to the launch pad
int launchpad_dup_fd(launchpad_t* this, int fd, int target_fd);

// Pass a file descriptor from a process to the launchpad.
int launchpad_pass_fd(launchpad_t* this, int fd, int target_fd);

// Create a pipe between the process and the launch pad.
int launchpad_pass_fd(launchpad_t* this, int fd, int target_fd);