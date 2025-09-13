# Hello, world!

This is the first tutorial in the series. It will show you how to create a simple "Hello, world!" program in C++ for skiftOS.

## Prerequisites

 - [skiftOS development environment](../building.md)

## Create a new project

First, create a new directory for your project and navigate to it:

```bash
# You need to be in the root of the skiftOS repository

$ mkdir -p src/tutorial/hello-world

$ cd src/tutorial/hello-world

# Open the project directory in your favorite text editor, for example VSCode
$ code .
```

## Write the manifest

Create a `manifest.json` file in the project directory:

```json
{
    "$schema": "https://schemas.cute.engineering/stable/cutekit.manifest.component.v1",
    "id": "hello-world",
    "type": "exe",
    "description": "Greets the world",
    "requires": [
        "karm-sys"
    ]
}
```

Here is a brief explanation of the fields in the manifest:

- `$schema`: The schema of the manifest file. It should always be `https://schemas.cute.engineering/stable/cutekit.manifest.component.v1`.
- `id`: The unique identifier of the component. It should be unique across all components.
- `type`: The type of the component. It should be `exe` for an executable.
- `description`: A brief description of the component.
- `requires`: A list of components that this component depends on.

## Write the source code

Create a `main.cpp` file in the project directory:

```cpp
#include <karm-sys/entry.h>

// The entry point of the program, skiftOS doesn't use `main` function
Async::Res<> entryPointAsync(Sys::Context &) {
    // Print "Hello, world!" to the console
    Sys::println("Hello, world!");

    // Return success
    co_return Ok();
}
```

## Build the project

Now, build the project using the `./skift.sh` script:

```bash
$ ./skift.sh build hello-world
```

## Run the program

Finally, run the program using the `./skift.sh` script:

```bash
$ ./skift.sh run hello-world
```

You should see "Hello, world!" printed to the console.

## Conclusion

Congratulations! You have created your first program for skiftOS.


## Going further

- [Next tutorial: Hello, GUI!](hello-gui.md)
- src/libs/karm-sys/chan.h - The system channel API is similar to stdio
