# Hello, GUI

In this tutorial, we will create a simple graphical user interface (GUI) application that displays a message.

## Prerequisites

- [skiftOS development environment](../building.md)
- [Hello, world!](hello-world.md)

## Create a new project

We will be starting from the `hello-world` project we created in the previous tutorial.

## Add the GUI library

First, we need to add the GUI library to the project. Open the `manifest.json` file in the project directory and add the `karm-ui` library to the `requires` field:

```json
{
    "$schema": "https://schemas.cute.engineering/stable/cutekit.manifest.component.v1",
    "id": "hello-world",
    "type": "exe",
    "description": "Greets the world",
    "requires": [
        "karm-sys",
        "karm-ui"
    ]
}
```

## Write the source code

Update the `main.cpp` file in the project directory to create a text view with the message "Hello, GUI!".

```cpp
#include <karm-sys/entry.h>
import Karm.Ui;

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    // Create a text view with the message "Hello, GUI!"
    auto app = Ui::text("Hello, GUI!");

    // Run the app
    co_return co_await Ui::runAsync(ctx, app);
}
```

## Build and run

Build and run the project:

```sh
./skift.sh builder run hello-world
```

You should see a window with the message "Hello, GUI!".

## Centering the text

You might have noticed that the text is not centered in the window. To center the text, we can use the `center` function:

```cpp
#include <karm-sys/entry.h>
import Karm.Ui;

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    // Create a text view with the message "Hello, GUI!" and center it
    auto app = Ui::text("Hello, GUI!") | Ui::center();

    // Run the app
    co_return co_await Ui::runAsync(ctx, app);
}
```

If you build and run the project again, you should see the text centered in the window.

## Conclusion

Congratulations! You have created your first GUI application for skiftOS

## Going further

Karm UI provides many more features for creating graphical user interfaces. You can explore the [Karm UI documentation](/libraries/karm-ui) to learn more about the available components and how to use them.
