# Building

This guide provides instructions for building skiftOS from source.

## Prerequisites

* Linux or macOS
* Clang (18 or later)
* Python (3.11.5 or later)
* Tools: `llvm`, `clang`, `ninja`, `nasm`, `qemu`
* **Installation:** Use your system's package manager (e.g., `apt` on Ubuntu) to install these dependencies.

## Instructions

0. **Prerequisites:**

    Verify that you have the required dependencies installed.

    <details>
    <summary>Ubuntu</summary>

    ```sh
    apt update

    apt install build-essential git ninja-build libsdl2-dev nasm gcc-multilib qemu-system-x86 mtools liburing-dev

    bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" llvm 17
    ```

    </details>

    <details>
    <summary>Arch Linux</summary>

    ```sh
    pacman -Syu git clang llvm nasm qemu-full gptfdisk mtools liburing sdl2
    ```

    </details>



1. **Clone the Repository:**
    ```sh
    git clone https://github.com/skift-org/skift.git

    cd skift
    ```

2. **Check Build Status (Optional):**

    Look at the build status badge for any potential issues:

    [![Checks (Ubuntu)](https://github.com/skift-org/skift/actions/workflows/checks-linux.yml/badge.svg)](https://github.com/skift-org/skift/actions/workflows/checks-linux.yml)
   
    [![Checks (Darwin)](https://github.com/skift-org/skift/actions/workflows/check-darwin.yml/badge.svg)](https://github.com/skift-org/skift/actions/workflows/check-darwin.yml)

4. **Set Up Build Environment:**

    ```sh
    ./skift.sh tools setup
    ```
    > **🛈 Note**<br> On Ubuntu, this step also installs dependencies.

5. **Verify Setup:**

   ```sh
   ./skift.sh tools doctor
   ```

6. **Download Dependencies:**

   ```sh
   ./skift.sh model install
   ```

7. **Build and Run:**

    - In Virtual Machine:
        ```sh
        ./skift.sh image start
        ```
        > **🛈 Note**<br> Requires a graphical environment and QEMU with graphics support (SDL or GTK).
        > For headless servers, build a disk image instead: `./skift.sh image build --format=hdd`


    - In simulator:
        ```sh
        ./skift.sh builder run --mixins=release hideo-shell.main
        ```

        > **🛈 Note**<br> The `--mixins=release` flag is optional and can be omitted for a debug build.


    **Enjoy!** You now have a working installation of skiftOS.

    **Need Help?** Join the [Discord server](https://discord.com/invite/gamGsfg) for assistance in the `#build-issues` channel.
