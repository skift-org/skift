<img src="logo-light.svg#gh-light-mode-only" height="24" />
<img src="logo-dark.svg#gh-dark-mode-only" height="24" />

# Roadmap

The following roadmap is always subject to change and is not a promise of any
kind. It is a rough outline of the features that are planned for the next
release.

## Milestone 0 - Minimum Viable Product

The aime of this milestone is tigten all the core framework and make then
work withing skiftOS.

- [x] Basic micro-kernel
- [x] Basic UEFI bootloader
- [ ] Basic system library with asynchonous I/O support
- [ ] Basic IPC library
- [x] Basic graphic library with subpixel font rendering
- [x] Basic user interface library
- [ ] Interactive graphical user interface shell

## Milestone 1 - Networking

This milestone is about adding networking support to the system.

- [ ] Simple virtio network driver
- [ ] Simple TCP/IP stack
- [ ] Simple TLS implementation
- [ ] Simple HTTP server & client
- [ ] Simple DNS client

## Milestone 2 - Storage

With this milestone we want to add support for storage devices.

- [ ] Simple virtio block driver
- [ ] Simple FAT32 filesystem support (read-only)
- [ ] Simple ext2 filesystem support (read-only)

## Milestone 3 - User Experience

With network and storage support, we can start to build a more complete user
experience. Some apps are already done but they need to be integrated into the
system.

- [ ] Apps should be able to be started from the shell
- [ ] Terminal emulator
- [ ] Text editor
- [x] Image viewer
- [x] Calculator
- [x] File manager
- [ ] Settings panel
- [ ] Clock
- [ ] Font manager

## Milestone X - Backlog

This is a list of features that are not planned for any specific milestone yet.

- [ ] Audio support
- [ ] Multi-core support
- [ ] Multi-user support
- [ ] Porting DOOM
- [ ] Better JPEG support
    - [ ] Chroma subsampling
    - [ ] Progressive mode
    - [ ] Arithmetic coding

