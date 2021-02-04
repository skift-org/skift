#!/bin/sh
VERSION = 1.2.9

echo "Getting ZLIB version: $VERSION"
git clone --depth 1 --branch v$VERSION https://github.com/madler/zlib sources