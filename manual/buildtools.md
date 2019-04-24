# Buildtools

## Name

automagicaly - a python3 script for building skiftOS.

## Synopsis

```sh

automagicaly [ACTION] [TARGET]

```

## Actions

### `run` - start the kernel in qemu

This action will:
   1. build all targets
   2. generate a iso file
   3. start qemu  

```sh

./automagicaly run

```

### `build` - Build a target and it's dependancies

```sh

./automagicaly build skift.hjert.kernel

```

### `clean` - Delete all build file of the target

```sh

./automagicaly clear skift.hjert.kernel

```

### `rebuild` - Clean and build a target

```sh

./automagicaly re-build skift.hjert.kernel

```

### The `-all` modifier

Append `-all` to apply an *action* to all targets.

```sh

./automagicaly build-all

```

## Exemples

```sh
# Run the operation system in qemu
./automagicaly run

# Show help
./automagicaly help

# Build a target
./automagicaly build skift.hjert.kernel

# For a simple build
./automagicaly build-all

# For a clean build (release)
./automagicaly rebuild-all
```