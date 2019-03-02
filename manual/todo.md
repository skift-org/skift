# TODO

## Build tools `buildtools.py`
 - [ ] Generating a SDK *(toolchain + prebuild distro for app-developper)*
 - [ ] Support of ports

## **Kernel** `packages/skift.hjert.kernel`

 - [ ] Proper cleanup of finish processes and threads (see: tasking.c)
 - [ ] Add support for kernel boot-time command-line arguments
 - [ ] Add support for HPET
 - [ ] Add support for IOAPIC

## **Framework** `packages/maker.skift.framework`
 - [ ] Drawing library
 - [ ] GUI toolkit

## **Compositor** `packages/maker.hideo.compositor`
 - [ ] Port to the new messaging system for handling:
   - [ ] Mouse events and dispatching
   - [ ] Keyboard events

 - [ ] Work on the protocol
   - [ ] Window creation/closing/resizing
   - [ ] Input events

## **Application** `packages/`
 - [ ] File manager
 - [ ] Terminal emulator (move console.c out of the kernel)

## **Ports** `packages/`
 - [x] nyancat (but old version)
 - [ ] Lua (need improvement in buildtools.py)