#!/usr/bin/env sh

set -e

export MYPY_CACHE_DIR=".cache/mypy"
export PYTHONPYCACHEPREFIX=$(pwd)/.cache/python
export CCACHE_DIR=".cache/ccache"

if [ -n "$CHECK" ];
then
    mypy --config .sk/.mypyconfig --pretty .sk/sk.py
fi
/usr/bin/env python3 .sk/sk.py $@
