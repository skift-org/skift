#!/bin/bash

set -e

# check for CUTEKIT_NOVENV
if [ "$CUTEKIT_NOVENV" == "1" ]; then
    echo "CUTEKIT_NOVENV is set, skipping virtual environment setup."
    exec cutekit $@
fi

if [ "$1" == "nuke-tools" ]; then
    rm -rf .cutekit/tools .cutekit/venv
    exit 0
fi

if [ ! -f .cutekit/tools/ready ]; then
    if [ ! "$1" == "setup" ]; then
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

    mkdir -p .cutekit
    if [ ! -d .cutekit/venv ]; then
        echo "Setting up Python virtual environment..."
        python3 -m venv .cutekit/venv
    fi
    source .cutekit/venv/bin/activate

    echo "Downloading CuteKit & Chatty..."
    if [ ! -d .cutekit/tools/cutekit ]; then
        git clone --depth 1 https://github.com/cute-engineering/cutekit .cutekit/tools/cutekit --branch "0.6.1"
    else
        echo "CuteKit already downloaded."
    fi

    if [ ! -d .cutekit/tools/chatty ]; then
        git clone --depth 1 https://github.com/cute-engineering/chatty .cutekit/tools/chatty
    else
        echo "Chatty already downloaded."
    fi

    echo "Installing Dependencies..."
    pip3 install -r meta/plugins/requirements.txt

    echo "Installing Tools..."
    pip3 install -e .cutekit/tools/cutekit
    pip3 install -e .cutekit/tools/chatty

    touch .cutekit/tools/ready
    echo "Done!"
fi

if [ "$1" == "setup" ]; then
    exit 0
fi

source .cutekit/venv/bin/activate
export PATH="$PATH:.cutekit/venv/bin"

cutekit $@
