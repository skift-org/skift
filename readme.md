
![Banner](manual/header.png)

<p align="center">
  <a href="https://discord.gg/gamGsfg">Discord</a> -
  <a href="https://skiftos.org/">Website</a> -
  <a href="https://github.com/skiftOS/skift/releases">Releases</a> -
  <a href="manual/readme.md">Manual</a> -
  <a href="manual/running_vm.md">Run as VM</a>
</p>
<p align="center">
  <b>Notice!</b> We've moved the project to C++ recently, so don't expect it to be idiomatic yet. There is still a lot of work to be done, and any help is very appreciated :)
</p>

# skiftOS

[![Deploy](https://github.com/skiftOS/skift/workflows/Deploy/badge.svg?branch=master)](https://github.com/skiftOS/skift/actions)
[![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/skiftOS/skift?include_prereleases)](https://github.com/skiftOS/skift/releases)
[![GitHub last commit](https://img.shields.io/github/last-commit/skiftOS/skift)](https://github.com/skiftOS/skift/commits)
[![GitHub issues](https://img.shields.io/github/issues-raw/skiftOS/skift)](https://github.com/skiftOS/skift/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/skiftOS/skift)](https://github.com/skiftOS/skift/pulls)

 - **skiftOS** is a simple, handmade operating system for the x86 platform, aiming for clean and pretty APIs while keeping the spirit of UNIX.
 - **skiftOS** features an UNIX-like terminal experience with many familiar UNIX utilities.
 - **skiftOS** is constantly updated, for the latest release `.img` of skiftOS, click [here](https://github.com/skiftOS/skift/releases/latest).
 - **skiftOS** is also pretty easy to build from source, just follow the [build guide](./manual/building.md).
 - **skiftOS** can function as a virtual machine, click [here](./manual/running_vm.md) to set up an instance on Virtual Box.
 - **skiftOS** is capable of running on real hardware.
 - **skiftOS** is a labor of love.
 - **skiftOS** features a graphical user interface (GUI), with a compositor.
 - **skiftOS** includes many applications like an [Image Viewer](./apps/image-viewer), a [Text Editor](./apps/text-editor), a [Terminal Emulator](./apps/terminal), [etc](./apps)...
 - **skiftOS** can [run DOOM :rage3:](https://github.com/skiftOS/port-doom)

# Screenshots

<p align="center">
<img src="manual/screenshots/2020-10-25.png" />
<br>
skiftOS running in QEMU 5.1
</p>

<br>

<p align="center">
<a href="https://www.youtube.com/watch?v=nMaLGPecwAg">
<img src="https://img.youtube.com/vi/nMaLGPecwAg/0.jpg" />
<br>
(Youtube) Systems with JT - SkiftOS (version 20.51)
</a>
</p>

<br>

<p align="center">
<a href="https://www.youtube.com/watch?v=qC2Qkt2wapg">
<img src="https://img.youtube.com/vi/qC2Qkt2wapg/0.jpg" />
<br>
(Youtube) Daimyo - Tribute to free softwares on SkiftOS.
</a>
</p>

# Links

- [Manual](manual/readme.md)
- [Building](manual/building.md) **(start here if you are new)**
- [Contributing](manual/contributing.md)
- [Code of conduct](manual/code_of_conduct.md)
- [Running on Virtual Box](manuals/running_vm.md) **(click here to experience skiftOS)**

# Acknowledgements

## Contributors

<a href="https://github.com/skiftOS/skift/graphs/contributors">
  <img src="https://contributors-img.web.app/image?repo=skiftOS/skift" />
</a>

## Projects

- [echfs](https://github.com/qword-os/echfs)
- [limine](https://github.com/limine-bootloader/limine) (BSD-2-Clause License)
- [lodepng](https://github.com/lvandeve/lodepng) (zlib License)
- [Material Design Icons](http://materialdesignicons.com/) (SIL OPEN FONT LICENSE )
- [std_truetype](https://github.com/nothings/stb) (MIT/Public Domain)
- [serenityOS AK utilities](https://github.com/SerenityOS/serenity): 
  while not directly used in skift, AK was a big inspiration in the creation of libutils.

# License

The skift operating system and its core components are licensed under the **MIT License**.

See [license.md](license.md)
