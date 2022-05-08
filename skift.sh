#!/usr/bin/env bash

set -e

export MYPY_CACHE_DIR=".cache/mypy"
export PYTHONPYCACHEPREFIX=$(pwd)/.cache/python

if [[ -v CHECK ]];
then
    mypy --config .build/.mypyconfig --pretty .build/runner.py
fi
/usr/bin/env python3 .build/runner.py $@
