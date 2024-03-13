# Karm UI

Karm UI is a library for creating graphical user interfaces for skiftOS.

> **🛈 Note**<br>
> This article is a work in progress, and is not yet complete. If you would like to contribute to the documentation, please refer to the [contributing guide](/contributing.md).

## Concepts

### Nodes

The basic building block of Karm UI is the node. A node is a graphical element that can be displayed on the screen. There are many types of nodes, such as text nodes, image nodes, and layout nodes.

Node are meant to be composed together to create complex user interfaces.

```cpp
auto app = Ui::vflow(
    Ui::text("Hello, world!"),
    Ui::image("image.png")
);
```

### Views

Views are leaf nodes that contain content to be displayed on the screen. They can be text views or image views.

```cpp
#include <karm-ui/view.h>

auto app = Ui::text("Hello, world!"); // A text view

auto app = Ui::image("image.png"); // An image view
```

### Layouts

Layouts are nodes that contain other nodes. They are used to arrange nodes in a specific way, such as in a grid or a list.

```cpp
#include <karm-ui/layout.h>

auto app = Ui::vflow(
    Ui::text("Hello, world!"),
    Ui::image("image.png")
); // A vertical flow layout

auto app = Ui::hflow(
    Ui::text("Hello, world!"),
    Ui::image("image.png")
); // A horizontal flow layout
```

### Decorators

Decorators are nodes that modify the appearance or behavior of other nodes. They can be used to add borders, padding, or other visual effects to nodes.

```cpp
#include <karm-ui/layout.h>

auto app = Ui::text("Hello, world!") | Ui::center(); // Center the text
```

In this example, the `center` decorator is used to center the text in the host.

## Going further

This document is only meant to provide a high-level overview of Karm UI. To learn more about the available components and how to use them, you can explore the source code at `src/libs/karm-ui`
