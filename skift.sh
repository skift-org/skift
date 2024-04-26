#!/bin/bash

set -e

function is_ubuntu() {
    if [ -f /etc/os-release ]; then
        grep -q "ubuntu" /etc/os-release
        return $?
    fi
    return 1
}

function is_darwin() {
    if [ "$(uname)" == "Darwin" ]; then
        return 0
    fi
    return 1
}

if [ "$EUID" -eq 0 ]; then
    echo "Please do not run this script as root."

    if [ "$CUTEKIT_ALLOW_ROOT" == "1" ]; then
        echo "CUTEKIT_ALLOW_ROOT is set, continuing..."
    else
        echo "If you know what you are doing, set CUTEKIT_ALLOW_ROOT=1 and try again."
        echo "Aborting."
        exit 1
    fi
fi

if [ "$CUTEKIT_NOVENV" == "1" ]; then
    echo "CUTEKIT_NOVENV is set, skipping virtual environment setup."
    exec cutekit $@
fi

if [ "$1" == "tools" -a "$2" == "nuke" ]; then
    rm -rf .cutekit/tools .cutekit/venv .cutekit/tools-ready
    exit 0
fi

if [ ! -f .cutekit/tools-ready ]; then
    if [ ! \( "$1" == "tools" -a "$2" == "setup" \) ]; then
        echo "Tools not installed."
        echo "This script will install the tooling required to build SkiftOS into $PWD/.cutekit"

        read -p "Do you want to continue? [Y/n] " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "Aborting."
            exit 1
        fi
    else
        echo "Installing tools..."
    fi

    if is_ubuntu; then
        echo "Detected Ubuntu, installing dependencies automatically..."
        sudo ./meta/scripts/setup-ubuntu.sh
    elif is_darwin; then
        echo "Detected macOS, installing dependencies automatically..."
        ./meta/scripts/setup-darwin.sh
    fi

    mkdir -p .cutekit
    if [ ! -d .cutekit/venv ]; then
        echo "Setting up Python virtual environment..."
        python3 -m venv .cutekit/venv
    fi
    source .cutekit/venv/bin/activate

    echo "Installing Tools..."
    pip3 install -r meta/plugins/requirements.txt

    touch .cutekit/tools-ready
    echo "Done!"
    exit 0
fi

if [ "$1" == "tools" -a "$2" == "setup" ]; then
    echo "Tools already installed."
    exit 0
fi

if is_darwin; then
    source ./meta/scripts/env-darwin.sh
fi

source .cutekit/venv/bin/activate
export PATH="$PATH:.cutekit/venv/bin"

cutekit $@
