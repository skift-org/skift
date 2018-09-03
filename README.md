<p align="center">
    <img src="doc/logo/logo256.png">
</p>

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
./TOOLS <action>
```

```shell
./TOOLS cleanall

./TOOLS buildall

./TOOLS rebuildall

./TOOLS run
```

```shell
./TOOLS <action> <target>
```

```shell
./TOOLS build test-app

./TOOLS clean test-app

./TOOLS rebuild test-app

./TOOLS run test-app
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

```c
typedef unsigned int FILE

FILE sk_file_open(const char * path, int flags)
void sk_file_close(FILE file)

void sk_file_read()
void sk_file_write()
```

```c
typedef unsigned int DIRECTORY

DIRECTORY sk_directory_open(const char * path, int flags)
void sk_directory_close(DIRECTORY directory)
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