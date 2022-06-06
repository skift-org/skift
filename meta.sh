#!/usr/bin/env sh

set -e

export MYPY_CACHE_DIR=".cache/mypy"
export PYTHONPYCACHEPREFIX=$(pwd)/.cache/python
export CCACHE_DIR=".cache/ccache"

if [ -n "$CHECK" ];
then
    mypy --config meta/.mypyconfig --pretty meta/__main__.py
fi
/usr/bin/env python3 meta $@
