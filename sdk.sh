#!/usr/bin/env sh

set -e

export MYPY_CACHE_DIR=".cache/mypy"
export PYTHONPYCACHEPREFIX=$(pwd)/.cache/python
export CCACHE_DIR=".cache/ccache"

if [ -n "$CHECK" ];
then
    mypy --config sdk/.mypyconfig --pretty sdk/__main__.py
fi
/usr/bin/env python3 sdk $@
