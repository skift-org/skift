#!/bin/bash

set -e

find src/ -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i
