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

There is three families of nodes in Karm UI:

 - Leaf nodes, such as text and image nodes, that contain content to be displayed on the screen.
 - Group nodes, such as flow and grid nodes, that contain other nodes.
 - Proxy nodes, such as decorators, that modify the appearance or behavior of other nodes by wrapping them.

### Composability

Nodes are immutable, meaning that they cannot be modified after they are created. Instead, new nodes are created by composing existing nodes together.

```cpp
auto app = Ui::text("Hello, world!") | Ui::center();
```

In this example, the `center` decorator is used to center the text in the host. Note that the original text node is not modified; instead, a new node is created by wrapping the original node with the decorator using the `|` operator.

### Tree structure

Nodes are organized in a tree structure, with each node having zero or more child nodes. The root node of the tree is the top-level node that represents the entire user interface.

```cpp
auto app = Ui::vflow(
    Ui::text("Hello, world!"),
    Ui::image("image.png")
);
```

In this example, the `vflow` node is the root node of the tree, and it contains two child nodes: a text node and an image node.

## Types of nodes

### Views

Views also known as Leaf nodes, are the most basic type of node. They contain content to be displayed on the screen, such as text or images. They are the content of the user interface.

```cpp
import Karm.Ui;

auto app = Ui::text("Hello, world!"); // A text view

auto app = Ui::image("image.png"); // An image view
```

### Layouts

Layouts are nodes that contain other nodes. They are used to arrange nodes in a specific way, such as in a grid or a list. There are two types of layout nodes: flow and grid.

```cpp
import Karm.Ui;

auto app = Ui::vflow(
    Ui::text("Hello, world!"),
    Ui::image("image.png")
); // A vertical flow layout

auto app = Ui::hflow(
    Ui::text("Hello, world!"),
    Ui::image("image.png")
); // A horizontal flow layout
```

**Flow Layouts**

A flow layout arranges its child nodes in a single row or column.

- `Ui::vflow(isize gaps, Math::Align align, auto... children)` : A vertical flow layout
- `Ui::hflow(isize gaps, Math::Align align, auto... children)` : A horizontal flow layout
- `<node> | Ui::grow(float factor)` : Expand the node to fill the available space, with a given factor.

**Grid Layouts**

A grid layout arranges its child nodes in a grid with a fixed number of rows and columns.

- `Child grid(GridStyle style, auto... children);` : A grid layout
- `<node> | Ui::cell(Math::Vec2i pos)` : Place the node in a specific cell of the grid.

### Decorators

Decorators are nodes that modify the appearance or behavior of other nodes. They can be used to add borders, padding, or other visual effects to nodes.

```cpp
import Karm.Ui;

auto app = Ui::text("Hello, world!") | Ui::center(); // Center the text
```

In this example, the `center` decorator is used to center the text in the host.

Here is a non exaustive list of decorators

**Visibility**

 - `<node> | Ui::cond(bool visible)` : Show or hide a node based on a condition.

**Bound and Growth**

 - `<node> | Ui::bound()` : Give a visual bound to the node.
 - `<node> | Ui::placed(Math::Recti place)` : Place the node at a specific location.
 - `<node> | Ui::grow(float factor)` : Expand the node to fill the available space, with a given factor.

**Alignment**

 - `<node> | Ui::align(Math::Align)` : Align the node in the host.
 - `<node> | Ui::center()` : Center the node in the host.
 - `<node> | Ui::start()` : Align the node to the start of the host.
 - `<node> | Ui::end()` : Align the node to the end of the host.
 - `<node> | Ui::fit()` : Fit the node to the size of the host.
 - `<node> | Ui::cover()` : Cover the host with the node.
 - `<node> | Ui::vcenter()` : Vertically center the node in the host.
 - `<node> | Ui::hcenter()` : Horizontally center the node in the host.
 - `<node> | Ui::vcenterFill()` : Vertically center the node in the host and fill the available space.
 - `<node> | Ui::hcenterFill()` : Horizontally center the node in the host and fill the available space.

**Size Management**

 - `<node> | Ui::sizing(Math::Vec2i min, Math::Vec2i max)` : Set the minimum and maximum size of the node.
 - `<node> | Ui::minSize(Math::Vec2i min)` : Set the minimum size of the node.
 - `<node> | Ui::maxSize(Math::Vec2i max)` : Set the maximum size of the node.
 - `<node> | Ui::pinSize(Math::Vec2i size)` : Set the size of the node.
 - `<node> | Ui::aspectRatio(f64 ratio)` : Set the aspect ratio of the node.

**Padding and Margin**

 - `<node> | Ui::insets(int insets)` : Add insets between the node and its parent.

## State Management

Uis are stateless by default, but you can use the `state` node to create stateful UIs.

```cpp
import Karm.Ui;

auto app = Ui::state(
    0,
    [](auto state, auto bind) {
        return Ui::vflow(
            Ui::text("Counter: {}", state),
            Ui::button(bind(state + 1) "Increment")
        );
    }
);
```

In this example, the `state` node is used to create a stateful UI that displays a counter and a button to increment the counter. The `state` node takes an initial state value and a function that returns a UI based on the current state value. The `bind` function is used to create a callback that updates the state when the button is clicked.

Behind the scenes, the `state` will re-render the UI whenever the state changes and reconcile the differences between the old and new UIs to update the screen.

## Going further

This document is only meant to provide a high-level overview of Karm UI. To learn more about the available components and how to use them, you can explore the source code at `src/libs/karm-ui`
