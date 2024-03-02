# Building

## Prerequisites

* Linux or macOS
* Clang (16 or later)
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
    pacman -Syu git clang llvm nasm qemu-full
    ```

    </details>



1. **Clone the Repository:**
    ```sh
    git clone https://github.com/skift-org/skift.git

    cd skift
    ```

2. **Check Build Status (Optional):**

    Look at the build status badge for any potential issues:

    [![Build](https://github.com/skift-org/skift/actions/workflows/checks.yml/badge.svg)](https://github.com/skift-org/skift/actions/workflows/checks.yml)

3. **Set Up Build Environment:**

    ```sh
    ./skift.sh tools setup
    ```
    > **🛈 Note**: On Ubuntu, this step also installs dependencies.

4. **Verify Setup:**

   ```sh
   ./skift.sh tools doctor
   ```

5. **Download Dependencies:**

   ```sh
   ./skift.sh model install
   ```

6. **Build and Run:**

   ```sh
   ./skift.sh image start
   ```
    > **🛈 Note**:
    >  Requires a graphical environment and QEMU with graphics support (SDL or GTK). For headless servers, build a disk image instead: `./skift.sh image build --format=hdd`

    **Enjoy!** You now have a working installation of skiftOS.

    **Need Help?** Join the [Discord server](https://discord.com/invite/gamGsfg) for assistance in the `#build-issues` channel.
