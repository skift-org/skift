#!/usr/bin/env bash

export PATH=$(brew --prefix llvm)/bin:$PATH
$@
