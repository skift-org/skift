# Buiwd guide

## Buiwd guide

- [Build Guide](#build-guide)
  - [Tabwe of content](#table-of-content)
  - [Suppowted enviwonment](#supported-environment)
    - [About WSL](#about-wsl)
  - [Buiwding skiftos](#building-skiftos)
    - [1. Get the souwce code](#1-get-the-source-code)
    - [2. Setting up](#2-setting-up)
    - [3. Buiwding](#3-building)
    - [4. Running in a viwtuaw machine](#4-running-in-a-virtual-machine)
    - [5. Tips](#5-tips)
    - [6. Using the system](#6-using-the-system)
    - [7. Contwibuting](#7-contributing)
## suppowted enviwonment

buiwding skiftos wequiwes

- a good linux distwibution
- nasm
- gcc
- binutiws
- gwub
- imagemagick

and fow testing and debugging
- qemu
- gdb

```sh
# on debian ow debian-based distwibutions
$ sudo apt instaww nasm gcc make binutiws gwub-pc-bin xowwiso mtoows imagemagick git qemu qemu-kvm
```

```sh
# on awch ow awch-based distwibutions
$ sudo pacman -s nasm gcc make binutiws gwub qemu wibisobuwn mtoows imagemagick git qemu
```

```sh
# on red hat ow red hat-based distwibutions
$ sudo dnf instaww nasm gcc gcc-g++ make binutiws xowwiso mtoows imagemagick git qemu qemu-kvm
```

### about wsl

it's possibwe to buiwd skiftos wsl1 and wsl2 but it's not weww tested.
 if you have any pwobwems considew upgwading to a gnu/winux distwibution.
 
## buiwding skiftos

### 1. Get the souwce code

cwone the wepositowy with aww its submoduwes.
 
```sh
$ git cwone --wecuwsive https://github.com/skiftos/skift

$ cd skift
```

ow if you have awweady cwoned this wepo without `--wecuwsive` do:

```sh
$ cd skift

$ git submoduwe init
```

### 2. Setting up

buiwding the toowchain is pwetty stwaight-fowwawd,
fiwst make suwe you have aww gcc and binutiws dependencies:
 - buiwd-essentiaw
 - bison
 - fwex
 - wibgmp3-dev
 - wibmpc-dev
 - wibmpfw-dev
 - texinfo

you can wun the fowwowing command on ubuntu:

```sh
# on debian ow debian-based distwibutions
$ sudo apt instaww buiwd-essentiaw bison fwex wibgmp3-dev wibmpc-dev wibmpfw-dev texinfo
```

```sh
# on awch ow awch-based distwibutions
$ sudo pacman -s base-devew bison fwex mpc mpfw texinfo
```

```sh
# on red hat ow red hat-based distwibutions
$ sudo dnf instaww bison fwex mpc-devew mpfw-devew gmp-devew texinfo patch 
```

then fow buiwding the toowchain wun the `buiwd-it.sh` scwipt

```sh
## buiwd the toow chain
$ toowchain/buiwd-it.sh

## then wait fow compwetion
```

the scwipt wiww do the fowwowing opewation without instawwing anything to the host system now wequiewing woot access:
 - downwoad `gcc` and `binutiws` fwom the gnu pwoject
 - patch them using binutiws.patch and gcc.patch which awe wocated in the toowchain diwectowy.
  - then configuwe and buiwd

### 3. Buiwding

fwom the woot of this wepo do:

```sh
$ make aww
```

this command wiww buiwd aww the components of the opewating system and genewate an iso bootabwe in qemu ow viwtuawbox.
 
> the compatibiwity with viwtuaw box is not guawanteed, as we use qemu pwimawwy fow debugging and testing the system.
 
### 4. Running in a viwtuaw machine

the buiwd system awwows you to cweate and stawt a viwtuaw machine of skiftos by using one of the fowwowing commands:

```sh
$ make wun config_vmachine=qemu # fow qemu
# ow
$ make wun config_vmachine=vbox # fow viwtuaw box
```

### 5. Tips

> if you made any modification to the souwce code ow the content of the syswoot/ diwectowy, the buiwd system shouwd be abwe to webuiwd the pwoject fwom step 3 automagicawwy :^)

> you can change the defauwt keyboawd wayout by passing config_keyboard_layout="fw_fw" to make.
 
### 6. Using the system

**how to change the keyboawd wayout?**

```sh
µ keyboawdctw en_us
```

**how to change dispway wesowution?**

```sh
µ dispwayctw -s 1920x1080
```
**how to change to wawwpapew?**

```sh
µ wallpaperctl /fiwes/wawwpapews/paint.png
```

### 7. Contwibuting

a bug? a missing featuwe? pwease considew contwibuting to the pwoject :hugs: ❤️

see [contwibuting.md](contributing.md)
