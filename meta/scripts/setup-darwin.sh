#!/usr/bin/env bash

if ! command -v brew &> /dev/null
then
    echo "Homebrew is not installed. Please install Homebrew first."
    echo '/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"'
    exit 1
fi

brew install --overwrite libmagic lld llvm nasm ninja pkg-config python3 qemu sdl2
