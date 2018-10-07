<p align="center">
    <img src="doc/logo/logo256.png">
</p>

<h1 align="center">skiftOS</h1>

**skiftOS** is a simple, handmade, build from scratch, 32bit operating system for intel x86 CPU.

**skiftOS** support virtual memory, multi processing with threads, and filesystem.

**skiftOS** provide a rich syscall interface for user application.

## What **skiftOS** look like

![screenshots](doc/screenshots/capture.png)

## Build-it

```sh
# Build the operating system
./TOOLS buildall

# Run it in qemu
./TOOLS run
```

## Features

## Kernel

- [x] IDT, GDT, IRQ and ISR
- [x] Memory managment
  - [x] Pagging
- [ ] Tasking (wip)
  - [ ] Sync (Wait, sleep, wakeup)
  - [ ] IPC (Shared memory, mailbox, pipes?) (wip)
  - [ ] ELF loader (wip)
  - [ ] System calls (wip)
  - [ ] Jump to user mode (wip)
- [ ] File System
  - [x] ATA pio driver
  - [x] RAM disk
  - [ ] Virtual file system (WIP)

### Libraries

- [ ] libc (wip)
  - [ ] stdio.h
  - [ ] stdlib.h
- [ ] libgfx (wip)
- [ ] libui

### Application

- [ ] Basic shell
- [ ] coreutils (ls, cat, ...)
- [ ] Compositor 
- [ ] Panels
- [ ] Desktop
- [ ] Terminal
- [ ] File browser
- [ ] Text editor
- [ ] Manual browser