#!/bin/bash

set -e

function is_ubuntu() {
    if [ -f /etc/os-release ]; then
        grep -q "ubuntu" /etc/os-release
        return $?
    fi
    return 1
}

if [ "$CUTEKIT_NOVENV" == "1" ]; then
    echo "CUTEKIT_NOVENV is set, skipping virtual environment setup."
    exec cutekit $@
fi

if [ "$1" == "tools" -a "$2" == "nuke" ]; then
    rm -rf .cutekit/tools .cutekit/venv
    exit 0
fi

if [ ! -f .cutekit/tools/ready ]; then
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
    fi

    mkdir -p .cutekit
    if [ ! -d .cutekit/venv ]; then
        echo "Setting up Python virtual environment..."
        python3 -m venv .cutekit/venv
    fi
    source .cutekit/venv/bin/activate

    echo "Installing Tools..."
    pip3 install -r meta/plugins/requirements.txt

    echo "Done!"
fi

if [ "$1" == "tools" -a "$2" == "setup" ]; then
    echo "Tools already installed."
    exit 0
fi

source .cutekit/venv/bin/activate
export PATH="$PATH:.cutekit/venv/bin"

cutekit $@
