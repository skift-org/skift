# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# v0.2.0 - **unreleased**

## Added
 - More witty comments to the kerne panic screen
 - Command line argument parser to framework
 - `all, color, list` and `help` options to `ls`
 - `filesystem_ioctl` juste for device file, but WIP

## Fixed
 - `\t` does't behaves as espected in the terminal
 - `sk_filesystem_read` was woking only once on regular files
 - `ls folder0 folder1 folder2` syntaxe wasn't working properly
 - #15 It was possible to create several files with the same name in the same directory
 - #16 Page fault when reading from a invalid file descriptor

# v0.1.0 
 - Initial release
