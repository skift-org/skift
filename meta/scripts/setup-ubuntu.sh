#!/bin/bash

set -e

export DEBIAN_FRONTEND=noninteractive

apt-get update -y --no-install-recommends
apt-get install -y --no-install-recommends ccache build-essential git ninja-build libsdl2-dev nasm gcc-multilib qemu-system-x86 mtools liburing-dev
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" llvm 19 all
