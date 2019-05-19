# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# v0.2.0 - **unreleased**

## Added
 - More witty comments to the kernel panic screen
 - Command line argument parser to framework
 - Path manipulation and parsing module to framework
 - Command line option to `ls`, 
 - `filesystem_ioctl` juste for device file, but WIP
 - `filesystem_link` and `filesystem_unlink`
 - `skift/iostream.h` a stdio like library
 - `skift/launchpad.h` a process creation API inspired by the fushia project
 - `cd` and `exit` command to the shell

## Removed
 - `skift/thread.h` all processes related function go to `skift/process.h`

## Fixed
 - `\t` does't behaves as espected in the terminal
 - `filesystem_read` was woking only once on regular files
 - `ls folder0 folder1 folder2` syntaxe wasn't working properly
 - `cat` was working on directory
 - `ls` was cat'ing file when apply to regular files
 - file name is no longer stored in the inode
 - #15 It was possible to create several files with the same name in the same directory
 - #16 Page fault when reading from a invalid file descriptor
 - Nested panic
 - The memory was free'ing the wrong physical page when doing a `memory_free` or `memory_unmap`

# v0.1.0 
 - Initial release
