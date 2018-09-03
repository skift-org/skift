
![logo](doc/logo/logo256.png)

# About skift**OS**
## Build infrastructure, kernel, and userspace

skift**OS** is a handmade operating system build from scratch.

## Featuring

- Multitasking
- File system
- Userspace

## 🏗 Build-it

```sh
# Build the operating system
./TOOLS buildall

# Run it in qemu
./TOOLS run
```

# Documentation - WIP

The userspace api

## `TOOLS` manual

```shell
./TOOLS build test-app

./TOOLS clean test-app

./TOOLS rebuild test-app

./TOOLS run test-app
```

```shell
./TOOLS cleanall

./TOOLS buildall

./TOOLS rebuildall

./TOOLS run
```

## About `manifest.json`

```json
{
    "id": "test-app",
    "type": "app",
    "libs": ["libc-userland", "libutils"],
    "strict": false
}
```

## "Standart" lib c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <...>
```

## File system

```c
#include <skift/io.h>
```

## Threads, process, and mutex

```c
#include <skift/thread.h>
#include <skift/mutex.h>
#include <skift/proc.h>
```

## IPC (Shared memory, Messaging and signals)

```c
#include <skift/ipc.h>
```