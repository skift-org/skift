#!/bin/bash
nasm -f elf32 crt0.s -o ../files/lib/crt0.o
nasm -f elf32 crti.s -o ../files/lib/crti.o
nasm -f elf32 crtn.s -o ../files/lib/crtn.o