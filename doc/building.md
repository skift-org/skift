<br>
<img src="logo-light.svg#gh-light-mode-only" height="24" />
<img src="logo-dark.svg#gh-dark-mode-only" height="24" />
<br>
<br>

# Building

> [!NOTE]
> If you are having trouble building skiftOS, feel free to ask for help in the [Discord server](https://discord.com/invite/gamGsfg)

To build skiftOS, you need to have a modern C/C++ compiler such as clang-16 installed on your computer. Additionally, you need to have the following tools installed: `python3`, `llvm`, `clang`, `ninja`, `nasm`, and `qemu`. Please refer to the documentation of these tools or use your package manager to install them.

### Step 1: Set up the build environment

First, set up the build environment by running the following command:

```sh
$ ./skift.sh tools setup
```

Then, check that everything is working correctly by running:

```sh
$ ./skift.sh tools doctor
```

### Step 2: Download third-party dependencies

After setting up the build environment, download the third-party dependencies by running the following command:

```sh
$ ./skift.sh model install
```

### Step 3: Build and run skiftOS

> [!NOTE]
> Running this step requires being in a graphical environment with a build of QEMU supporting graphics either via SDL or GTK. If you are using a headless server, you can use the `./skift.sh image build --format=hdd` command to build a disk image.

Finally, build and run skiftOS by running the following command:

```sh
$ ./skift.sh image start
```

### Step 4: Enjoy!

If all these steps are completed successfully, you should have a fully built and operational skiftOS ready for use. Enjoy! 😊
