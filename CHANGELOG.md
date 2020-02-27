# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# v0.2.0 - **unreleased**

## Added
 - More witty comments to the kernel panic screen
 - Command line argument parser module to framework
 - Path manipulation and parsing module to framework
 - Command line option to some coreutils
 - `call`, `link`, `unlink`, `rename` syscalls
 - `skift/io/stream.h` a stdio like library, with support for buffered I/O
 - `skift/launchpad.h` a process creation API inspired by the fushia project (wip)
 - `cd`, `exit`, `mv` command to the shell
 - Devices files: `/dev/framebuffer`, `/dev/textmode`, `/dev/proc`
 - `lsproc` utility for listing running processes
 - `displayctl` utility for getting, setting and listing graphics modes
 - Some graphic demos `lines`, `colors`, `gfxtest`
 - Mouse cursors, fonts and wallpapers in `/res`
 - Support for FPU
 - Support for FIFO/NAMED_PIPES
 - Support for errno

## Changes
 - Task and processes are now the same thing
 - libgfx now support alpha blending
 - Messages can now be sended synchronously
 - moved out of the kernel the terminal rendering code to `/bin/term`
 - Moved the toolchain from i686-elf to i686-pc-skift

## Fixed
 - `\t` does't behaves as espected in the terminal
 - `filesystem_read` was woking only once on regular files
 - `ls folder0 folder1 folder2` syntaxe wasn't working properly
 - `cat` was working on directory
 - `ls` was cat'ing file when apply to regular files
 - File name is no longer stored in the inode
 - #15 It was possible to create several files with the same name in the same directory
 - #16 Page fault when reading from a invalid file descriptor
 - Nested panic are now supported properly no more stack overflow
 - The memory manager was free'ing the wrong physical page when doing a `memory_free` or `memory_unmap`
 - Processes are now properly cleaned-up

# v0.1.0
 - Initial release
